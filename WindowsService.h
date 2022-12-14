#pragma once

#include <ServiceBase.h>
#include <string>

// Default service start options.
#define SERVICE_START_TYPE       SERVICE_AUTO_START

// List of service dependencies (none)
#define SERVICE_DEPENDENCIES     L""

// Default name of the account under which the service should run
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\LocalService"

// Default password to the service account name
#define SERVICE_PASSWORD         NULL

// Configuration file
#define SERVICE_CONFIG_FILE      L"config.cfg"

// Command to run as a service
#define SERVICE_CMD              L"service"

// Command to run as a stand-alone process
#define PROCESS_CMD              L"run"

// Service name
#define SERVICE_NAME             L"Astra.Keyboard"

// Service name as displayed in MMC
#define SERVICE_DISP_NAME        L"Astra.Keyboard"

// Service description as displayed in MMC
#define SERVICE_DESC             L"?????? ??? ?????????? ??????????????? ???????????"

using namespace std;

class CSampleService: public CServiceBase
{
  public:
    CSampleService(PCWSTR pszServiceName,
                   BOOL fCanStop = TRUE,
                   BOOL fCanShutdown = TRUE,
                   BOOL fCanPauseContinue = FALSE
                  );
    virtual ~CSampleService();

    virtual void OnStart(DWORD dwArgc, PWSTR *pszArgv);

    virtual void OnStop();

    static DWORD __stdcall ServiceRunner(void* self);

    void Run();
  private:
    BOOL m_bIsStopping;
    HANDLE m_hHasStoppedEvent;
    wstring m_wstrParam;
};

