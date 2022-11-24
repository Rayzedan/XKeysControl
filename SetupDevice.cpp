#include "SetupDevice.h"
#include <iostream>

BYTE buffer[80]{};

SetupDevice::SetupDevice() 
{
	TEnumHIDInfo info[128];
	result = 0;
	hnd = 0;
	long count = 0;
	int pid;
	result = EnumeratePIE(0x5F3, info, count);

	if (result != 0)
	{
		if (result == 102) {
			std::cout << "No PI Engineering Devices Found\n";
		}
		else {
			std::cout << "Error finding PI Engineering Devices\n";
		}
	}
	else if (count == 0) {
		std::cout << "No PI Engineering Devices Found\n";
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
			if (result != 0)
			{
				std::cout << "Error setting up PI Engineering Device\n";
			}
			else
			{
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
		//Key Index (in decimal)
		//Bank 1
		//Columns-->
		//  0   8   16  24  32  40  48  56  64  72
		//  1   9   17  25  33  41  49  57  65  73

		//  3   11      27  35  43  51      67  75
		//  4   12      28  36  44  52      68  76
		//  5   13      29  37  45  53      69  77
		//  6   14      30  38  46  54      70  78
		//  7   15      31  39  47  55      71  79

		//Bank 2
		//Columns-->
		//  80	88	96	104	112	120	128	136	144	152
		//  81	89	97	105	113	121	129	137	145	153

		//  83	91		107	115	123	131		147	155
		//  84	92		108	116	124	132		148	156
		//  85	93		109	117	125	133		149	157
		//	86	94		110	118	126	134		150	158
		//  87  95      111 119 127 135     151 159
		std::cout << "Change LED\n";
		int wlen = GetWriteLength(hnd);
		int indexButton = 0, indexState = 0;
		for (int i = 0; i < wlen; i++)
		{
			buffer[i] = 0;
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

		buffer[1] = 181; //0xb5
		//get key index
		//get text box text
	}
}

SetupDevice::~SetupDevice() 
{
	std::cout << "DELETE SETUP DEVICE\n";
}

void SetupDevice::setLED(int indexButton, int indexState)
{
	buffer[1] = 181;
	std::cout << "INDEX BUTTON - " << indexButton << " INDEX STATE - " << indexState;
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
		result = 404;
		while (result == 404)
		{
			result = WriteData(hnd, buffer);
		}
	}
	else {
		buffer[2] = indexButton;
		buffer[3] = 0;
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
