#pragma once
#include "PieHid64.h"

class SetupDevice
{
public:
	SetupDevice();
	virtual ~SetupDevice();
    void setLED(int indexButton, int indexState, bool isStatusGood);
	void setAllRed();
	void setAllBlue();
	void setTimeoutDevice(DWORD timeout);
	static bool getCurrentState();
private:
	static void writeDeviceData(int bufferKey,int indexButton, int indexState);
	static DWORD __stdcall HandleDataEvent(UCHAR* pData, DWORD deviceID, DWORD error);
	static DWORD __stdcall HandleErrorEvent(DWORD deviceID, DWORD status);
	static void __stdcall installDevice();
	static void __stdcall callbackSetLED(int indexButton, int indexState);

};

