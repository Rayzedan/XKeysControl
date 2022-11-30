#include "SetupDevice.h"
#include <iostream>
#include <map>

struct buttonData
{
	bool isEnable;
	int indexState;
};


static char dataStr2[256];
static BYTE buffer[80];
static BYTE lastpData[80];
static long hnd;

static std::map<int, buttonData>buttonsMap;
bool isDeviceEnable = false;
DWORD timeoutDevice = 30;

SetupDevice::SetupDevice() 
{	
	std::cout << "Create SetupDevice\n";
}

SetupDevice::~SetupDevice() 
{
	//std::cout << "DELETE SETUP DEVICE\n";
}

void SetupDevice::initialDevice()
{
	installDevice();
}

bool SetupDevice::getCurrentState()
{
	if (isDeviceEnable)
		return true;
	else {
		return false;
		Sleep(timeoutDevice * 1000);
		installDevice();
	}		
}

void SetupDevice::writeDeviceData(int indexButton, int indexState)
{
	//std::cout << "write device data\n";
	DWORD result = 0;
	int wlen = GetWriteLength(hnd);
	for (int i = 0; i < wlen; ++i)
	{
		buffer[i] = 0;
	}
	buffer[1] = 181;
	buffer[2] = indexButton;
	buffer[3] = indexState;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}
}

void SetupDevice::installDevice()
{
	std::cout << "setup device...\n";
	TEnumHIDInfo info[128];
	DWORD result = 0;
	hnd = 0;
	long count = 0;
	int pid = 0;
	result = EnumeratePIE(0x5F3, info, count);	
	if (result != 0)
	{
		if (result == 102) {
			//std::cout << "No PI Engineering Devices Found\n";
		}
		else {
			std::cout << "Error finding PI Engineering Devices\n";
		}
	}
	else if (count == 0) {		
		Sleep(timeoutDevice * 1000);
		std::cout << "No PI Engineering Devices Found\n";
		std::cout << "Trying to find a device...\n";
		result = EnumeratePIE(0x5F3, info, count);
	}
	for (long i = 0; i < count; ++i) {
		pid = info[i].PID; //get the device pid		
		int hidusagepage = info[i].UP; //hid usage page
		int version = info[i].Version;
		int writelen = GetWriteLength(info[i].Handle);

		if ((hidusagepage == 0xC && writelen == 36))
		{
			hnd = info[i].Handle; //handle required for piehid.dll calls
			result = SetupInterfaceEx(hnd);
			std::cout << "Find new PI Engineering Device\n";
			isDeviceEnable = true;			
		}
	}
	if (result == 0) {
		std::cout << "trying setup device to default...\n";
		//Turn on the data callback
		result = SetDataCallback(hnd, HandleDataEvent);
		result = SetErrorCallback(hnd, HandleErrorEvent);
		SuppressDuplicateReports(hnd, true);
		DisableDataCallback(hnd, false); //turn on callback in the case it was turned off by some other command

		int wlen = GetWriteLength(hnd);		
		for (int i = 0; i < wlen; ++i)
		{
			buffer[i] = 0;
		}
		for (int i = 0; i < 256; ++i)
		{
			dataStr2[i] = 0;
		}
		for (int i = 0; i < 80; ++i)
		{
			lastpData[i] = 0;
		}
		buffer[1] = 179; //0xb3
		buffer[2] = 6; //6=green, 7=red
		buffer[3] = 1;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
		
		buffer[1] = 182;
		buffer[2] = 0; //0 for bank 1, 1 for bank 2
		buffer[3] = 255;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}

		buffer[2] = 1; //0 for bank 1, 1 for bank 2
		buffer[3] = 0;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
		buffer[1] = 181; //0xb5
		if (buttonsMap.size() > 0) {
			for (auto const &item:buttonsMap)
			{
				if (!item.second.isEnable) {
					writeDeviceData(item.first, 0);
					writeDeviceData(item.first + 80, 1);
				} else if (item.second.indexState == 1) {
					writeDeviceData(item.first + 80, 0);
					writeDeviceData(item.first , 1);
				} else {
					writeDeviceData(item.first, 0);
					writeDeviceData(item.first + 80, 2);
				}
			}
		}
	}
}

void SetupDevice::callbackSetLED(int indexButton, int indexState)
{
	DWORD result = 0;
	std::cout << "callback set led was used\n";
	buffer[1] = 181;	
	if (indexState == 1 && buttonsMap.count(indexButton) != 0) {
		if (buttonsMap[indexButton].indexState == 0) {
			writeDeviceData(indexButton + 80, 1);
		}
	}
}

void SetupDevice::setLED(int indexButton, int indexState, bool isStatusGood)
{
	DWORD result = 0;
	buffer[1] = 181;
	std::cout << "INDEX BUTTON - " << indexButton << " INDEX STATE - " << indexState << std::endl;
	if (isStatusGood) {
		buttonsMap[indexButton].isEnable = true;
		if (indexState == 1) {
			writeDeviceData(indexButton + 80, 0);
			writeDeviceData(indexButton, indexState);
			buttonsMap[indexButton].indexState = 1;
		}
		else {
			buttonsMap[indexButton].indexState = 0;
			writeDeviceData(indexButton, 0);
			writeDeviceData(indexButton + 80, indexState);
		}
	}
	else {
		writeDeviceData(indexButton,0);
		writeDeviceData(indexButton + 80, 1);
		buttonsMap[indexButton].isEnable = false;
		buttonsMap[indexButton].indexState = 0;
	}
	
}

DWORD SetupDevice::HandleDataEvent(UCHAR* pData, DWORD deviceID, DWORD error)
{	
	int d = _itoa_s(pData[1], dataStr2, 10);
	int readlength = 0;
	int maxcols = 10;
	int maxrows = 8;
	for (int i = 0; i < maxcols; i++) //loop for each column of button data (Max Cols)
	{
		for (int j = 0; j < maxrows; j++) //loop for each row of button data (Max Rows)
		{
			int temp1 = pow(2, j);
			int keynum = maxrows * i + j; //0 based index

			int state = 0; //0=was up and is up, 1=was up and is down, 2= was down and is down, 3=was down and is up 
			if (((pData[i + 3] & temp1) != 0) && ((lastpData[i + 3] & temp1) == 0))
				state = 1;
			
			if (state == 1)
				callbackSetLED(keynum, 1);
		}
	}

	for (int i = 0; i < readlength; i++)
	{
		lastpData[i] = pData[i];  //save it for comparison on next read
	}
	//end Buttons

	//error handling
	if (error == 307)
	{
		DisableDataCallback(hnd, true);
		CleanupInterface(hnd);
		//CloseInterface(hnd);
		MessageBeep(MB_ICONHAND);
		std::cout << "Device disconnected\n";
		isDeviceEnable = false;
		while (!isDeviceEnable)
		{
			Sleep(timeoutDevice*1000);
			SetupDevice::installDevice();
		}
	}
	return TRUE;
}

DWORD SetupDevice::HandleErrorEvent(DWORD deviceID, DWORD status)
{
	MessageBeep(MB_ICONHAND);
	std::cout << "Error from error callback\n";
	return TRUE;
}

void SetupDevice::setTimeoutDevice(DWORD timeout)
{
	timeoutDevice = timeout;
}

void SetupDevice::setAllRed()
{
	DWORD result = 0;
	buffer[1] = 179; //0xb3
	buffer[2] = 6; //6=green, 7=red
	buffer[3] = 0;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}

	buffer[1] = 179; //0xb3
	buffer[2] = 7; //6=green, 7=red
	buffer[3] = 1;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}


	buffer[0] = 0;
	buffer[1] = 182;
	buffer[2] = 1; //0 for bank 1, 1 for bank 2
	buffer[3] = 255;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}

	buffer[2] = 0; //0 for bank 1, 1 for bank 2
	buffer[3] = 0;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}
}

void SetupDevice::setAllBlue()
{
	DWORD result = 0;
	buffer[1] = 179; //0xb3
	buffer[2] = 7; //6=green, 7=red
	buffer[3] = 0;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}

	buffer[1] = 179; //0xb3
	buffer[2] = 6; //6=green, 7=red
	buffer[3] = 1;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}

	buffer[0] = 0;
	buffer[1] = 182;
	buffer[2] = 0; //0 for bank 1, 1 for bank 2
	buffer[3] = 255;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}

	buffer[2] = 1; //0 for bank 1, 1 for bank 2
	buffer[3] = 0;
	result = 404;
	while (result == 404)
	{
		result = WriteData(hnd, buffer);
	}
}