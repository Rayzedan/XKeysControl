#pragma once
#include "PieHid64.h"

class SetupDevice
{
public:
	SetupDevice();
	virtual ~SetupDevice();
<<<<<<< HEAD
<<<<<<< HEAD
	void setLED(int indexButton, int indexState);
=======
=======
>>>>>>> b29218e (merge branches)
    void setLED(int indexButton, int indexState);
>>>>>>> da350ce (New branch)
	void setAllRed();
	void setAllBlue();
	void setTimeoutDevice(DWORD timeout);
	void initialDevice();
private:

=======
	void setLED(int indexButton, int indexState);
private:
	DWORD result;
	long hnd;
>>>>>>> 4df2694 (init project)
};

