#include <regex>
#include <tchar.h>
#include "StdAfx.h"
#include "WindowsService.h"
#include <ServiceInstaller.h>
#include <Windows.h>

int wmain(int argc, wchar_t* argv[])
{
	setlocale(LC_ALL, "Russian");
	// Service parameters
	DWORD dwSvcStartType = SERVICE_START_TYPE;
	PCWSTR wsSvcAccount = SERVICE_ACCOUNT;
	PCWSTR wsSvcPwd = SERVICE_PASSWORD;
	PCWSTR wsConfigFullPath = SERVICE_CONFIG_FILE;
	WCHAR wsServiceParams[MAX_PATH] = SERVICE_CMD;
	CSampleService service(SERVICE_NAME);

	if (argc > 1) {
		if (_wcsicmp(L"install", argv[1]) == 0) {
			try {
				for (int i = 2; i < argc; i++) {
					PWSTR arg = argv[i];
					if (arg[0] == '-') {
						if (_wcsicmp(L"-start-type", arg) == 0) {
							if (argc > i) {
								PCWSTR wsStartType = argv[++i];
								if (regex_match(wsStartType, wregex(L"[2-4]")))
									dwSvcStartType = _wtol(wsStartType);
								else
									throw exception("выберите режим установки службы (2-4)");
							}
							else {
								throw exception("не выбран режим установки");
							}
						}
						else {
							char errMsg[MAX_PATH];
							_snprintf_s(errMsg, _countof(errMsg), _TRUNCATE, "unknown parameter: %S", arg);

							throw exception(errMsg);
						}
					}
				}

				InstallService(
					SERVICE_NAME,               // Name of service
					SERVICE_DISP_NAME,          // Display name
					SERVICE_DESC,               // Description
					wsServiceParams,            // Command-line parameters to pass to the service
					dwSvcStartType,             // Service start type
					SERVICE_DEPENDENCIES,       // Dependencies
					wsSvcAccount,               // Service running account
					wsSvcPwd,                   // Password of the account
					TRUE,                       // Register with Windows Event Log, so our log messages will be found in Event Viewer
					1,                          // We have only one event category, "Service"
					NULL                        // No separate resource file, use resources in main executable for messages (default)
				);
			}
			catch (exception const& ex) {
				wprintf(L"Couldn't install service: %S", ex.what());
				return 1;
			}
			catch (...) {
				wprintf(L"Couldn't install service: unexpected error");
				return 2;
			}
		}
		else if (_wcsicmp(L"uninstall", argv[1]) == 0) {
			UninstallService(SERVICE_NAME);
		}
		else if (_wcsicmp(SERVICE_CMD, argv[1]) == 0) {
			CSampleService service(SERVICE_NAME);

			service.SetCommandLine(argc, argv);

			if (!CServiceBase::Run(service)) {
				DWORD dwErr = GetLastError();
				wprintf(L"Service failed to run with error code: 0x%08lx\n", dwErr);
				return dwErr;
			}
		}
		else if (_wcsicmp(PROCESS_CMD, argv[1]) == 0) {
			CSampleService service(SERVICE_NAME);

			service.SetCommandLine(argc, argv);

			service.Run();
		}
		else {
			wprintf(L"Unknown parameter: %s\n", argv[1]);
		}
	}
	else {
		wprintf(L"\nAstra.XKeysDriver\n\n");
		wprintf(L"Parameters:\n\n");
		wprintf(L" install [-start-type <2..4>]\n  - to install the service.\n");
		wprintf(L"    типы запуска службы:\n");
		wprintf(L"     2 - служба автоматически запускается диспетчером управления службами во время запуска системы.\n");
		wprintf(L"     3 - служба должна быть запущена вручную или путем вызова функции StartService из другого процесса.\n");
		wprintf(L"     4 - служба будет установлена ​​в отключенном состоянии.\n");
		wprintf(L" \n run - запускать как обычный процесс\n");
		wprintf(L" uninstall - удалить службу.\n");
	}
	return 0;
}
