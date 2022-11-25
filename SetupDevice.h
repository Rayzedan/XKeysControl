#pragma once
#include "PieHid64.h"

class SetupDevice
{
public:
	SetupDevice();
	virtual ~SetupDevice();
<<<<<<< HEAD
	void setLED(int indexButton, int indexState);
=======
    void setLED(int indexButton, int indexState);
>>>>>>> da350ce (New branch)
	void setAllRed();
	void setAllBlue();
	void setTimeoutDevice(DWORD timeout);
	void initialDevice();
private:

};

