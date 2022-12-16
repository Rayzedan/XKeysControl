#pragma once
#include "PieHid64.h"

class SetupDevice
{
public:
	SetupDevice();
	virtual ~SetupDevice();
	SetupDevice(const SetupDevice& copy) = delete;
	SetupDevice& operator=(const SetupDevice& client) = delete;
    void setLED(int indexButton, int indexState, bool isStatusGood);
	void setAllRed();
	void setAllBlue();
	void setTimeoutDevice(DWORD timeout);
	static bool getCurrentState();
	static void __stdcall installDevice();
private:
	static void writeDeviceData(int bufferKey,int indexButton, int indexState);
	static DWORD __stdcall HandleDataEvent(UCHAR* pData, DWORD deviceID, DWORD error);
	static DWORD __stdcall HandleErrorEvent(DWORD deviceID, DWORD status);	
	static void __stdcall callbackSetLED(int indexButton, int indexState);

};

