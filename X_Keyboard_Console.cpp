#include <iostream>
#include "OpcUaClient.h"
#include <iostream>
#include <tchar.h>


int __cdecl _tmain()
{
	setlocale(LC_ALL, "Russian");
	OpcUaClient* worker = new OpcUaClient();

	worker->subLoop("opc.tcp://localhost:62544");
	//delete worker;
	return 0;
}
