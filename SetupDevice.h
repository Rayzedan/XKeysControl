#pragma once
#include "PieHid64.h"

class SetupDevice
{
public:
	SetupDevice();
	virtual ~SetupDevice();
	void setLED(int indexButton, int indexState);
private:
	DWORD result;
	long hnd;
};

