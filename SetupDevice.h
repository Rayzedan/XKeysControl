#pragma once
#include "PieHid64.h"

class SetupDevice
{
public:
	SetupDevice();
	virtual ~SetupDevice();
	void setLED(int indexButton, int indexState);
	void setAllRed();
	void setAllBlue();
	void setTimeoutDevice(DWORD timeout);
private:

};

