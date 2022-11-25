#include "SetupDevice.h"
#include <iostream>
#include <map>

BYTE buffer[80]{};
BYTE lastpData[80];
DWORD result;
long hnd;
DWORD __stdcall HandleDataEvent(UCHAR* pData, DWORD deviceID, DWORD error);
DWORD __stdcall HandleErrorEvent(DWORD deviceID, DWORD status);
void installDevice();
void callbackSetLED(int indexButton, int indexState);
int readlength = 0;
std::map<int, int>buttonsMap;
bool isDeviceEnable = false;
DWORD timeoutDevice = 30;

SetupDevice::SetupDevice() 
{	
	
}

SetupDevice::~SetupDevice() 
{
	std::cout << "DELETE SETUP DEVICE\n";
}

void SetupDevice::initialDevice()
{
	installDevice();
}

void installDevice()
{
	std::cout << "setup device...\n";
	TEnumHIDInfo info[128];
	result = 0;
	hnd = 0;
	long count = 0;
	int pid;
	result = EnumeratePIE(0x5F3, info, count);
	
	while (result!=0)
	{
		Sleep(timeoutDevice * 1000);
		std::cout << "No PI Engineering Devices Found\n";
		std::cout << "Trying to find a device...\n";
		result = EnumeratePIE(0x5F3, info, count);
	}
	//if (result != 0)
	//{
	//	if (result == 102) {
	//		//std::cout << "No PI Engineering Devices Found\n";
	//	}
	//	else {
	//		//std::cout << "Error finding PI Engineering Devices\n";
	//	}
	//}
	//else if (count == 0) {
	//	//std::cout << "No PI Engineering Devices Found\n";
	//}
	for (long i = 0; i < count; ++i) {
		pid = info[i].PID; //get the device pid

		int hidusagepage = info[i].UP; //hid usage page
		int version = info[i].Version;
		int writelen = GetWriteLength(info[i].Handle);

		if ((hidusagepage == 0xC && writelen == 36))
		{
			hnd = info[i].Handle; //handle required for piehid.dll calls
			result = SetupInterfaceEx(hnd);
			if (result != 0)
			{
				//std::cout << "Error setting up PI Engineering Device\n";
			}
			else
			{
				isDeviceEnable = true;
				switch (pid)
				{
				case 1089:
					std::cout << "Found Device: XK-80, PID=1089 (PID #1)\n";
					break;
				case 1090:
					std::cout << "Found Device: XK-80, PID=1090 (PID #2)\n";
					break;
				case 1091:
					std::cout << "Found Device: XK-80, PID=1091 (PID #3)\n";
					break;
				case 1250:
					std::cout << "Found Device: XK-80, PID=1250 (PID #4)\n";
					break;
				case 1121:
					std::cout << "Found Device: XK-60, PID=1121 (PID #1)\n";
					break;
				case 1122:
					std::cout << "Found Device: XK-60, PID=1122 (PID #2)\n";
					break;
				case 1123:
					std::cout << "Found Device: XK-60, PID=1123 (PID #3)\n";
					break;
				case 1254:
					std::cout << "Found Device: XK-60, PID=1254 (PID #4)\n";
					break;
				default:
					std::cout << "Unknown device found\n";
					break;
				}
			}
		}
	}
	if (result == 0) {
		//Turn on the data callback
		result = SetDataCallback(hnd, HandleDataEvent);
		result = SetErrorCallback(hnd, HandleErrorEvent);
		SuppressDuplicateReports(hnd, true);
		DisableDataCallback(hnd, false); //turn on callback in the case it was turned off by some other command

		int wlen = GetWriteLength(hnd);
		int indexButton = 0, indexState = 0;
		for (int i = 0; i < wlen; i++)
		{
			buffer[i] = 0;
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
		//get key index
		//get text box text
	}
}

void callbackSetLED(int indexButton, int indexState)
{	
	buffer[1] = 181;	
	if (indexState == 1 && buttonsMap.count(indexButton) != 0) {
		if (buttonsMap[indexButton] == 0) {
			buffer[2] = indexButton + 80;
			buffer[3] = 1;
			result = 404;
			while (result == 404)
			{
				result = WriteData(hnd, buffer);
				std::cout << "result write - " << result << std::endl;
			}
		}
	}
}

void SetupDevice::setLED(int indexButton, int indexState)
{
	buffer[1] = 181;
	//std::cout << "INDEX BUTTON - " << indexButton << " INDEX STATE - " << indexState;
	if (indexState == 1) {
		buffer[2] = indexButton + 80;
		buffer[3] = 0;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
		buffer[2] = indexButton;
		buffer[3] = indexState;
		buttonsMap[indexButton] = 1;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
	}
	else {
		buffer[2] = indexButton;
		buffer[3] = 0;
		buttonsMap[indexButton] = 0;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
		buffer[2] = indexButton + 80;
		buffer[3] = indexState;
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
	}
}

DWORD __stdcall HandleDataEvent(UCHAR* pData, DWORD deviceID, DWORD error)
{	
	char dataStr2[256];
	_itoa_s(pData[1], dataStr2, 10);

	//Buttons
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
			else if (((pData[i + 3] & temp1) != 0) && ((lastpData[i + 3] & temp1) != 0))
				state = 2;
			else if (((pData[i + 3] & temp1) == 0) && ((lastpData[i + 3] & temp1) != 0))
				state = 3;

			//Perform action based on key number, consult P.I. Engineering SDK documentation for the key numbers
			switch (keynum)
			{
			case 0: //button 0 (top left)
				if (state == 1) //key was pressed
				{
					std::cout << "key 1 was pressed\n";
					callbackSetLED(keynum, 1);
				}
				break;
			case 1: //button 1
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				break;
			case 3: //button 3
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				break;
			case 4: //button 4
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				break;
			case 5: //button 5
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 6: //button 6
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 7: //button 7 
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons
			case 8: //button 8
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 9: //button 9
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 10: //button 10
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 11: //button 11
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 12: //button 12
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 13: //button 13
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 14: //button 14
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 15: //button 15
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons
			case 16: //button 16
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 17: //button 17
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 18: //button 18
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 19: //button 19
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 20: //button 20
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 21: //button 21
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 22: //button 22
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 23: //button 23
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons
			case 24: //button 24
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 25: //button 25
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 26: //button 26
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 27: //button 27
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 28: //button 28
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 29: //button 29
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 30: //button 30
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 31: //button 31
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons
			case 32: //button 32
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 33: //button 33
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 34: //button 34
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 35: //button 35
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons                             
			case 36: //button 36
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 37: //button 37
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 38: //button 38
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;


			case 39: //button 39
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons  
			case 40: //button 40
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 41: //button 41
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 42: //button 42
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 43: //button 43
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 44: //button 44
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 45: //button 45
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 46: //button 46
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 47: //button 47
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons		     
			case 48: //button 48
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 49: //button 49
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 50: //button 50
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 51: //button 51
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 52: //button 52
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 53: //button 53
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;

			case 54: //button 54
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 55: //button 55
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
				//Next column of buttons
			case 56: //button 56
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 57: //button 57
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 58: //button 58
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 59: //button 59
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;
			case 60: //button 60
				if (state == 1) //key was pressed
				{
					callbackSetLED(keynum, 1);
				}
				else if (state == 3) //key was released
				{
					//do release action
				}
				break;		
			}

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
		CleanupInterface(hnd);
		MessageBeep(MB_ICONHAND);
		std::cout << "Device disconnected\n";
		isDeviceEnable = false;
		while (!isDeviceEnable)
		{
			Sleep(timeoutDevice*1000);
			installDevice();
		}
	}
	return TRUE;
}

DWORD __stdcall HandleErrorEvent(DWORD deviceID, DWORD status)
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