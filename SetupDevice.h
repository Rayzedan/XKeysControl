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
	void initialDevice();
	bool getCurrentState();
private:
};

