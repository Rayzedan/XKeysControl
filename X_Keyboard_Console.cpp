#include <iostream>
#include "OpcUaClient.h"
#include <iostream>
#include <tchar.h>


int __cdecl _tmain()
{
	setlocale(LC_ALL, "Russian");
	OpcUaClient* worker = new OpcUaClient();

	worker->subLoop();
	//delete worker;
	return 0;
}
