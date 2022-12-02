#include "SetupDevice.h"
#include <iostream>
#include <map>

struct buttonData
{
	bool isEnable;
	int indexState;
};


BYTE buffer[80];
BYTE lastpData[80];
long hnd;
long count = 0;
int pid = 0;
std::map<int, buttonData>buttonsMap;
bool isDeviceEnable = false;
DWORD timeoutDevice = 30;

SetupDevice::SetupDevice()
{
	//std::cout << "Create module Setup Device...\n";
}

SetupDevice::~SetupDevice()
{
	//std::cout << "Delete module Setup Device...\n";
	isDeviceEnable = false;
	buttonsMap.clear();
}

bool SetupDevice::getCurrentState()
{
	if (isDeviceEnable)
		return true;
	else {		
		Sleep(timeoutDevice * 1000);
		installDevice();
		return false;
	}
}

void SetupDevice::writeDeviceData(int bufferKey, int indexButton, int indexState)
{
	//std::cout << "write device data\n";
	DWORD result = 0;
	int wlen = GetWriteLength(hnd);
	for (int i = 0; i < wlen; ++i)
	{
		buffer[i] = 0;
	}
	buffer[1] = bufferKey;
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
	TEnumHIDInfo info[128];
	char errordescription[100];
	//std::cout << "Setup device...\n";

	DWORD result = 0;
	result = EnumeratePIE(0x5F3, info, count);
	if (result != 0)
	{
		isDeviceEnable = false;
		if (result == 102) {
			//std::cout << "No PI Engineering Devices Found\n";
		}
		else {
			//std::cout << "Error finding PI Engineering Devices\n";
		}
	}
	else if (count == 0) {
		//std::cout << "No device found...\n";
	}
	else {
		//std::cout << "Found device...\n";
		isDeviceEnable = true;
		for (long i = 0; i < count; ++i) {
			pid = info[i].PID; //get the device pid		
			int hidusagepage = info[i].UP; //hid usage page
			int version = info[i].Version;
			int writelen = GetWriteLength(info[i].Handle);
			if ((hidusagepage == 0xC && writelen == 36))
			{
				hnd = info[i].Handle; //handle required for piehid.dll calls
				result = SetupInterfaceEx(hnd);
				//std::cout << "Find new PI Engineering Device\n";
			}
		}
		//Turn on the data callback	
		result = SetDataCallback(hnd, HandleDataEvent);
		result = SetErrorCallback(hnd, HandleErrorEvent);
		SuppressDuplicateReports(hnd, true);
		DisableDataCallback(hnd, false); //turn on callback in the case it was turned off by some other command
		if (result != 0) {
			GetErrorString(result, errordescription, 100);
			//std::cout << "error description - " << errordescription << std::endl;
		}

		int wlen = GetWriteLength(hnd);
		for (int i = 0; i < wlen; ++i)
		{
			buffer[i] = 0;
		}
		writeDeviceData(179, 6, 1);
		writeDeviceData(182, 0, 255);
		writeDeviceData(182, 1, 0);
		if (buttonsMap.size() > 0) {
			for (auto const& item : buttonsMap)
			{
				if (!item.second.isEnable) {
					writeDeviceData(181, item.first, 0);
					writeDeviceData(181, item.first + 80, 1);
				}
				else if (item.second.indexState == 1) {
					writeDeviceData(181, item.first + 80, 0);
					writeDeviceData(181, item.first, 1);
				}
				else {
					writeDeviceData(181, item.first, 0);
					writeDeviceData(181, item.first + 80, 2);
				}
			}
		}
	}
	
}

void SetupDevice::callbackSetLED(int indexButton, int indexState)
{
	DWORD result = 0;
	//std::cout << "callback set led was used\n";
	if (indexState == 1 && buttonsMap.count(indexButton) != 0) {
		if (buttonsMap[indexButton].indexState == 0) {
			writeDeviceData(181, indexButton + 80, 1);
		}
	}
}

void SetupDevice::setLED(int indexButton, int indexState, bool isStatusGood)
{
	DWORD result = 0;
	//std::cout << "INDEX BUTTON - " << indexButton << " INDEX STATE - " << indexState << std::endl;
	if (isStatusGood) {
		buttonsMap[indexButton].isEnable = true;
		if (indexState == 1) {
			writeDeviceData(181, indexButton + 80, 0);
			writeDeviceData(181, indexButton, indexState);
			buttonsMap[indexButton].indexState = 1;
		}
		else {
			buttonsMap[indexButton].indexState = 0;
			writeDeviceData(181, indexButton, 0);
			writeDeviceData(181, indexButton + 80, indexState);
		}
	}
	else {
		writeDeviceData(181, indexButton, 0);
		writeDeviceData(181, indexButton + 80, 1);
		buttonsMap[indexButton].isEnable = false;
		buttonsMap[indexButton].indexState = 0;
	}

}

DWORD SetupDevice::HandleDataEvent(UCHAR* pData, DWORD deviceID, DWORD error)
{
	char dataStr2[256];
	int readlength = 0;
	if (pData != nullptr) {
		//std::cout << "pData != nullptr\n";
		int d = _itoa_s(pData[1], dataStr2, 10);

		int maxcols = 10;
		int maxrows = 8;
		for (int i = 0; i < maxcols; ++i) //loop for each column of button data (Max Cols)
		{
			for (int j = 0; j < maxrows; ++j) //loop for each row of button data (Max Rows)
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

		for (int i = 0; i < readlength; ++i)
		{
			lastpData[i] = pData[i];  //save it for comparison on next read
		}
	}


	//error handling
	if (error == 307)
	{
		CleanupInterface(hnd);
		MessageBeep(MB_ICONHAND);
		//std::cout << "Device disconnected\n";
		isDeviceEnable = false;
	}
	return TRUE;
}

DWORD SetupDevice::HandleErrorEvent(DWORD deviceID, DWORD status)
{
	MessageBeep(MB_ICONHAND);
	//std::cout << "Error from error callback\n";
	return TRUE;
}

void SetupDevice::setTimeoutDevice(DWORD timeout)
{
	timeoutDevice = timeout;
}

void SetupDevice::setAllRed()
{
	DWORD result = 0;
	writeDeviceData(179, 6, 0);
	writeDeviceData(179, 7, 1);
	writeDeviceData(182, 1, 255);
	writeDeviceData(182, 0, 0);
}

void SetupDevice::setAllBlue()
{
	DWORD result = 0;
	writeDeviceData(179, 7, 0);
	writeDeviceData(179, 6, 1);
	writeDeviceData(182, 0, 255);
	writeDeviceData(182, 1, 0);
}