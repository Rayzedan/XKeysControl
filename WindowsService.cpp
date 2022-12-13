#include "stdafx.h"
#include "WindowsService.h"
#include "event_ids.h"
#include <iostream>
#include "OpcUaClient.h"

OpcUaClient* worker = new OpcUaClient();

CSampleService::CSampleService(PCWSTR pszServiceName,
	BOOL fCanStop,
	BOOL fCanShutdown,
	BOOL fCanPauseContinue) :
	CServiceBase(pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue, MSG_SVC_FAILURE, CATEGORY_SERVICE)
{
	setlocale(LC_ALL, "Russian");
	m_bIsStopping = FALSE;
	m_hHasStoppedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hHasStoppedEvent == NULL) {
		throw GetLastError();
	}
}

void CSampleService::OnStart(DWORD /* useleses */, PWSTR* /* useless */)
{
	const wchar_t* wsConfigFullPath = SERVICE_CONFIG_FILE;
	bool bRunAsService = true;
	// Log a service start message to the Application log.
	WriteLogEntry(L"Astra.Keyboard запускается...", EVENTLOG_INFORMATION_TYPE, MSG_STARTUP, CATEGORY_SERVICE);
	if (m_argc > 1) {
		bRunAsService = (_wcsicmp(SERVICE_CMD, m_argv[1]) == 0);
	}
	else {
		WriteLogEntry(L"Astra.Keyboard:\nNo run mode specified.", EVENTLOG_ERROR_TYPE, MSG_STARTUP, CATEGORY_SERVICE);
		throw exception("no run mode specified");
	}

	if (bRunAsService) {
		// Add the main service function for execution in a worker thread.
		if (!CreateThread(NULL, 0, ServiceRunner, this, 0, NULL))
			WriteLogEntry(L"Astra.Keyboard не может создать рабочий поток.", EVENTLOG_ERROR_TYPE, MSG_STARTUP, CATEGORY_SERVICE);

	}
	else
		CSampleService::ServiceRunner(this);

}

CSampleService::~CSampleService()
{

}

void CSampleService::Run()
{
	OnStart(0, NULL);
}

DWORD __stdcall CSampleService::ServiceRunner(void* self)
{
	CSampleService* pService = (CSampleService*)self;
	pService->WriteLogEntry(L"Astra.Keyboard успешно запущена.", EVENTLOG_INFORMATION_TYPE, MSG_STARTUP, CATEGORY_SERVICE);

	// Periodically check if the service is stopping.
	for (bool once = true; !pService->m_bIsStopping; once = false) {
		if (once) {
			// Log multi-line message
			pService->WriteLogEntry(L"Astra.Keyboard успешно работает.\n", EVENTLOG_INFORMATION_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
			worker->initialRequest();
		}
		worker->stopSession();
		switch (worker->getCurrentState()) {
		case -1:
			//std::cout << "Невозможно прочитать конфигурационный файл\n";
			pService->WriteLogEntry(L"Невозможно прочитать конфигурационный файл", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
			break;
		case 0:
			//std::cout << "Служба работает в штатном режиме\n";
			pService->WriteLogEntry(L"Служба работает в штатном режиме", EVENTLOG_INFORMATION_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
			break;
		case 1:
			//std::cout << "Модуль клиента не отвечает\n";
			pService->WriteLogEntry(L"Модуль OPC-клиента отключён", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
			break;
		case 2:
			//std::cout << "Модуль клиента не отвечает\n";
			pService->WriteLogEntry(L"Передача данных прервана", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
			break;
		default:
			//std::cout << "Неизвестная ошибка\n";
			pService->WriteLogEntry(L"Неизвестная ошибка", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
			break;
		}
	}
	// Signal the stopped event.
	SetEvent(pService->m_hHasStoppedEvent);
	pService->WriteLogEntry(L"Astra.Keyboard остановлена.", EVENTLOG_INFORMATION_TYPE, MSG_SHUTDOWN, CATEGORY_SERVICE);
	return 0;
}


void CSampleService::OnStop()
{
	// Log a service stop message to the Application log.
	WriteLogEntry(L"Astra.Keyboard отключается...", EVENTLOG_INFORMATION_TYPE, MSG_SHUTDOWN, CATEGORY_SERVICE);
	m_bIsStopping = TRUE;
}
