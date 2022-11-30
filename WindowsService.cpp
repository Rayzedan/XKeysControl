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

    if (m_hHasStoppedEvent == NULL)
    {
        throw GetLastError();
    }
}

void CSampleService::OnStart(DWORD /* useleses */, PWSTR* /* useless */)
{
    //const wchar_t* wsConfigFullPath = SERVICE_CONFIG_FILE;
    //bool bRunAsService = true;

    //// Log a service start message to the Application log.
    //WriteLogEntry(L"Astra.XKeysDriver is starting...", EVENTLOG_INFORMATION_TYPE, MSG_STARTUP, CATEGORY_SERVICE);

    //if (m_argc > 1)
    //{
    //    bRunAsService = (_wcsicmp(SERVICE_CMD, m_argv[1]) == 0);

    //    // Check if the config file was specified on the service command line
    //    if (m_argc > 2) // the argument at 1 should be "run mode", so we start at 2
    //    {
    //        if (_wcsicmp(L"-config", m_argv[2]) == 0)
    //        {
    //            if (m_argc > 3)
    //            {
    //                wsConfigFullPath = m_argv[3];
    //            }
    //            else
    //            {
    //                throw exception("no configuration file name");
    //            }
    //        }
    //    }
    //}
    //else
    //{
    //    WriteLogEntry(L"Astra.XKeysDriver:\nNo run mode specified.", EVENTLOG_ERROR_TYPE, MSG_STARTUP, CATEGORY_SERVICE);
    //    throw exception("no run mode specified");
    //}

    //try
    //{
    //    // Here we would load configuration file
    //    // but instead we're just writing to event log the configuration file name
    //    wstring infoMsg = L"Astra.XKeysDriver\n The service is pretending to read configuration from ";
    //    infoMsg += wsConfigFullPath;
    //    WriteLogEntry(infoMsg.c_str(), EVENTLOG_INFORMATION_TYPE, MSG_STARTUP, CATEGORY_SERVICE);
    //}
    //catch (exception const& e)
    //{
    //    WCHAR wszMsg[MAX_PATH];

    //    _snwprintf_s(wszMsg, _countof(wszMsg), _TRUNCATE, L"Astra.XKeysDriver\nError reading configuration %S", e.what());

    //    WriteLogEntry(wszMsg, EVENTLOG_ERROR_TYPE, MSG_STARTUP, CATEGORY_SERVICE);
    //}

    //if (bRunAsService)
    //{
    //    WriteLogEntry(L"Astra.XKeysDriver will run as a service.", EVENTLOG_INFORMATION_TYPE, MSG_STARTUP, CATEGORY_SERVICE);

    //    // Add the main service function for execution in a worker thread.
    //    if (!CreateThread(NULL, 0, ServiceRunner, this, 0, NULL))
    //    {
    //        WriteLogEntry(L"Astra.XKeysDriver couldn't create worker thread.", EVENTLOG_ERROR_TYPE, MSG_STARTUP, CATEGORY_SERVICE);
    //    }
    //}
    //else
    //{
        std::cout << "Astra.XKeysDriver is running as a regular process.\n";

        CSampleService::ServiceRunner(this);
    //}
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
    std::cout << "Astra.XKeysDriver has started.\n";
    pService->WriteLogEntry(L"Astra.XKeysDriver has started.", EVENTLOG_INFORMATION_TYPE, MSG_STARTUP, CATEGORY_SERVICE);

    // Periodically check if the service is stopping.
    for (bool once = true; !pService->m_bIsStopping; once = false)
    {
        if (once)
        {
            // Log multi-line message
            std::cout << "Astra.XKeysDrivere is working:\n";
            pService->WriteLogEntry(L"Astra.XKeysDrivere is working:\n", EVENTLOG_INFORMATION_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
            worker->initialRequest();         
        }
        switch (worker->getCurrentState())
        {
        case -1:
            std::cout << "Невозможно прочитать конфигурационный файл\n";
            pService->WriteLogEntry(L"Невозможно прочитать конфигурационный файл", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
            break;
        case 0:
            std::cout << "Служба работает в штатном режиме\n";
            pService->WriteLogEntry(L"Служба работает в штатном режиме", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
            break;
        case 1:
            std::cout << "Модуль клиента не отвечает\n";
            pService->WriteLogEntry(L"Модуль клиента не отвечает", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
            break;
        default:
            std::cout << "Неизвестная ошибка\n";
            pService->WriteLogEntry(L"Неизвестная ошибка", EVENTLOG_ERROR_TYPE, MSG_OPERATION, CATEGORY_SERVICE);
            break;
        }
        // Just pretend to do some work
        //Sleep(5000);
    }

    std::cout << "Astra.XKeysDriver has stopped.\n";
    // Signal the stopped event.
    SetEvent(pService->m_hHasStoppedEvent);
    pService->WriteLogEntry(L"Astra.XKeysDriver has stopped.", EVENTLOG_INFORMATION_TYPE, MSG_SHUTDOWN, CATEGORY_SERVICE);

    return 0;
}

void CSampleService::OnStop()
{
    std::cout << "Stop session...\n";
    // Log a service stop message to the Application log.
    WriteLogEntry(L"Astra.XKeysDriver is stopping", EVENTLOG_INFORMATION_TYPE, MSG_SHUTDOWN, CATEGORY_SERVICE);
    // Indicate that the service is stopping and wait for the finish of the
    // main service function (ServiceWorkerThread).
    m_bIsStopping = TRUE;
    worker->stopSession();

    if (WaitForSingleObject(m_hHasStoppedEvent, INFINITE) != WAIT_OBJECT_0)
    {
        throw GetLastError();
    }
}
