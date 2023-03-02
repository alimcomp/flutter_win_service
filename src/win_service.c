
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>
#include "include/result_struct.h"
#include "include/win_service.h"

typedef struct _NOTIFY_CONTEXT
{
    PTSTR pszServiceName;
} NOTIFY_CONTEXT, *PNOTIFY_CONTEXT;

SERVICE_STATUS_HANDLE g_ServiceStatusHandle;
SERVICE_STATUS g_ssStatus;
HANDLE g_StopEvent;
DWORD g_CurrentState = 0;
DWORD g_SystemShutdown = 0;

static Dart_Port_DL dart_port = 0;
static Dart_Port_DL status_change_dart_port = 0;
LPWSTR _serviceName = L"";

LPTSTR GetLastErrorText()
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&lpMsgBuf,
        0,
        NULL);
    LocalFree(lpMsgBuf);
    return (LPTSTR)lpMsgBuf;
}

void ReportStatus(DWORD state)
{
    g_CurrentState = state;
    SERVICE_STATUS serviceStatus = {
        SERVICE_WIN32_OWN_PROCESS,
        g_CurrentState,
        state == SERVICE_START_PENDING ? 0 : SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN,
        NO_ERROR,
        0,
        0,
        0,
    };
    SetServiceStatus(g_ServiceStatusHandle, &serviceStatus);

    if (dart_port != 0)
    {
        ServiceStatusStruct *result = (ServiceStatusStruct *)malloc(sizeof(ServiceStatusStruct));

        result->dwCurrentState = serviceStatus.dwCurrentState;

        Dart_CObject msg;
        msg.type = Dart_CObject_kInt64;

        msg.value.as_int64 = (intptr_t)(result);

        Dart_PostCObject_DL(dart_port, &msg);
    }
}
DWORD WINAPI HandlerEx(DWORD control, DWORD eventType, void *eventData, void *context)
{
    switch (control)
    {
        // Entrie system is shutting down.
    case SERVICE_CONTROL_SHUTDOWN:
        g_SystemShutdown = 1;
        // continue...
    // Service is being stopped.
    case SERVICE_CONTROL_STOP:
        ReportStatus(SERVICE_STOP_PENDING);
        SetEvent(g_StopEvent);
        break;
        // Ignoring all other events, but we must always report service status.
    default:
        ReportStatus(g_CurrentState);
        break;
    }
    return NO_ERROR;
}

void ReportErrorStatus(DWORD errorCode)
{

    g_CurrentState = SERVICE_STOPPED;
    SERVICE_STATUS serviceStatus = {
        SERVICE_WIN32_OWN_PROCESS,
        g_CurrentState,
        SERVICE_CONTROL_STOP,
        ERROR_SERVICE_SPECIFIC_ERROR,
        errorCode,
        0,
        0};
    SetServiceStatus(g_ServiceStatusHandle, &serviceStatus);
}

void ReportProgressStatus(DWORD state, DWORD checkPoint, DWORD waitHint)
{
    g_CurrentState = state;
    SERVICE_STATUS serviceStatus = {
        SERVICE_WIN32_OWN_PROCESS,
        g_CurrentState,
        state == SERVICE_START_PENDING ? 0 : SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN,
        NO_ERROR,
        0,
        checkPoint,
        waitHint,
    };
    SetServiceStatus(g_ServiceStatusHandle, &serviceStatus);
}

void WINAPI ServiceMain(DWORD argc, LPTSTR *argv)
{
    g_ServiceStatusHandle = RegisterServiceCtrlHandlerEx(_serviceName, &HandlerEx, NULL);

    // Startup code.
    ReportStatus(SERVICE_START_PENDING);
    g_StopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    ReportStatus(SERVICE_RUNNING);

    while (WaitForSingleObject(g_StopEvent, INFINITE) != WAIT_OBJECT_0)
    {
    }

    ReportStatus(SERVICE_STOP_PENDING);

    /* Here finalize service...
    Save all unsaved data etc., but do it quickly.
    If g_SystemShutdown, you can skip freeing memory etc. */
    CloseHandle(g_StopEvent);
    ReportStatus(SERVICE_STOPPED);
}
// Create a string with last error message

ServiceStatusStruct get_service_status(LPWSTR serviceName)
{
    struct ServiceStatusStruct result;
    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    schSCManager = OpenSCManager(
        NULL,                 // machine (NULL == local)
        NULL,                 // database (NULL == default)
        SC_MANAGER_ALL_ACCESS // access required
    );
    if (schSCManager)
    {
        schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);

        if (schService)
        {

            if (QueryServiceStatus(schService, &g_ssStatus))
            {

                result.dwServiceType = g_ssStatus.dwServiceType;
                result.dwCurrentState = g_ssStatus.dwCurrentState;
                result.dwControlsAccepted = g_ssStatus.dwControlsAccepted;
                result.dwWin32ExitCode = g_ssStatus.dwWin32ExitCode;
                result.dwServiceSpecificExitCode = g_ssStatus.dwServiceSpecificExitCode;
                result.dwCheckPoint = g_ssStatus.dwCheckPoint;
                result.dwWaitHint = g_ssStatus.dwWaitHint;
                result.status = true;
            }
            else
            {
                result.status = false;
                result.code = GetLastError();
                result.message = GetLastErrorText();
            }
            CloseServiceHandle(schService);
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
}

ResultStruct DoStopSvc(LPWSTR serviceName)
{
    struct ResultStruct result;
    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    SERVICE_STATUS_PROCESS ssp;
    DWORD dwStartTime = GetTickCount();
    DWORD dwBytesNeeded;
    DWORD dwTimeout = 30000; // 30-second time-out
    DWORD dwWaitTime;

    // Get a handle to the SCM database.

    schSCManager = OpenSCManager(
        NULL,                   // local computer
        NULL,                   // ServicesActive database
        SC_MANAGER_ALL_ACCESS); // full access rights

    if (schSCManager)
    {
        schService = OpenService(
            schSCManager, // SCM database
            serviceName,  // name of service
            SERVICE_ALL_ACCESS);

        if (schService)
        {

            if (QueryServiceStatusEx(
                    schService,
                    SC_STATUS_PROCESS_INFO,
                    (LPBYTE)&ssp,
                    sizeof(SERVICE_STATUS_PROCESS),
                    &dwBytesNeeded))
            {
                if (ssp.dwCurrentState == SERVICE_STOPPED)
                {
                    result.status = false;
                    result.code = 0;
                    result.message = L"service already stoped";
                }
                else if (ssp.dwCurrentState == SERVICE_STOP_PENDING)
                {
                    // If a stop is pending, wait for it.

                    while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
                    {
                        printf("Service stop pending...\n");

                        // Do not wait longer than the wait hint. A good interval is
                        // one-tenth of the wait hint but not less than 1 second
                        // and not more than 10 seconds.

                        dwWaitTime = ssp.dwWaitHint / 10;

                        if (dwWaitTime < 1000)
                            dwWaitTime = 1000;
                        else if (dwWaitTime > 10000)
                            dwWaitTime = 10000;

                        Sleep(dwWaitTime);

                        if (QueryServiceStatusEx(
                                schService,
                                SC_STATUS_PROCESS_INFO,
                                (LPBYTE)&ssp,
                                sizeof(SERVICE_STATUS_PROCESS),
                                &dwBytesNeeded))
                        {
                            if (ssp.dwCurrentState == SERVICE_STOPPED)
                            {
                                result.status = true;
                                result.code = 0;
                                result.message = L"service stoped successfully";
                                break;
                            }

                            else if (GetTickCount() - dwStartTime > dwTimeout)
                            {
                                printf("Service stop timed out.\n");

                                result.status = false;
                                result.code = 1;
                                result.message = L"timeout";
                                break;
                            }
                        }
                        else
                        {
                            result.status = false;
                            result.code = GetLastError();
                            result.message = GetLastErrorText();
                            break;
                        }
                    }
                }
                else
                {
                    // Send a stop code to the service.
                    if (ControlService(
                            schService,
                            SERVICE_CONTROL_STOP,
                            (LPSERVICE_STATUS)&ssp))
                    {
                        // Wait for the service to stop.

                        while (ssp.dwCurrentState != SERVICE_STOPPED)
                        {
                            Sleep(ssp.dwWaitHint);
                            if (QueryServiceStatusEx(
                                    schService,
                                    SC_STATUS_PROCESS_INFO,
                                    (LPBYTE)&ssp,
                                    sizeof(SERVICE_STATUS_PROCESS),
                                    &dwBytesNeeded))
                            {
                                if (ssp.dwCurrentState == SERVICE_STOPPED)
                                {
                                    result.status = true;
                                    result.code = 0;
                                    result.message = L"service stoped successfully";
                                    break;
                                }
                                else if (GetTickCount() - dwStartTime > dwTimeout)
                                {
                                    result.status = false;
                                    result.code = 1;
                                    result.message = L"timeout";
                                    break;
                                }
                            }
                            else
                            {
                                result.status = false;
                                result.code = GetLastError();
                                result.message = GetLastErrorText();
                                break;
                            }
                        }
                    }
                    else
                    {
                        result.status = false;
                        result.code = GetLastError();
                        result.message = GetLastErrorText();
                    }
                }
            }
            else
            {
                result.status = false;
                result.code = GetLastError();
                result.message = GetLastErrorText();
            }

            CloseServiceHandle(schService);
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
}

ResultStruct report_service_error(LPWSTR serviceName, DWORD errorCode)
{
    return DoStopSvc(serviceName);
    // struct ResultStruct result;
    // SC_HANDLE schService;
    // SC_HANDLE schSCManager;

    // SERVICE_STATUS ssStatus;

    // schSCManager = OpenSCManager(
    //     NULL,                 // machine (NULL == local)
    //     NULL,                 // database (NULL == default)
    //     SC_MANAGER_ALL_ACCESS // access required
    // );
    // if (schSCManager)
    // {
    //     schService = OpenService(
    //         schSCManager,      // SCManager database
    //         serviceName,       // name of service
    //         SERVICE_ALL_ACCESS // desired access
    //     );                     // no password

    //     if (schService)
    //     {
    //         g_CurrentState = SERVICE_STOPPED;
    //         SERVICE_STATUS serviceStatus = {
    //             SERVICE_WIN32_OWN_PROCESS,
    //             g_CurrentState,
    //             0,
    //             ERROR_SERVICE_SPECIFIC_ERROR,
    //             errorCode,
    //             0,
    //             0,
    //         };

    //         if (ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus))
    //         {
    //             result.status = true;
    //             result.code = 0;
    //         }
    //         else
    //         {
    //             result.status = false;
    //             result.code = GetLastError();
    //             result.message = GetLastErrorText();
    //         }
    //         CloseServiceHandle(schService);
    //     }
    //     else
    //     {
    //         result.status = false;
    //         result.code = GetLastError();
    //         result.message = GetLastErrorText();
    //     }

    //     CloseServiceHandle(schSCManager);
    // }
    // else
    // {
    //     result.status = false;
    //     result.code = GetLastError();
    //     result.message = GetLastErrorText();
    // }
    // SleepEx(10000, true);

    // return result;
}

// BOOL WINAPI ControlHandler(DWORD dwCtrlType)
// {
//     switch (dwCtrlType)
//     {
//     case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
//     case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode

//       //  ServiceStop();
//         return TRUE;
//         break;

//     }
//     return FALSE;
// }

ResultStruct start_service(LPWSTR serviceName)
{

    struct ResultStruct result;
    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    SERVICE_STATUS ssStatus;
    DWORD dwOldCheckPoint;
    DWORD dwStartTickCount;
    DWORD dwWaitTime;

    schSCManager = OpenSCManager(
        NULL,                 // machine (NULL == local)
        NULL,                 // database (NULL == default)
        SC_MANAGER_ALL_ACCESS // access required
    );
    if (schSCManager)
    {
        schService = OpenService(
            schSCManager,      // SCManager database
            serviceName,       // name of service
            SERVICE_ALL_ACCESS // desired access
        );                     // no password

        if (schService)
        {

            // service opend so try to start it
            if (!StartService(schService, 0, NULL))
            {
                result.status = false;
                result.code = GetLastError();
                result.message = GetLastErrorText();
            }
            else
            {
                // service gone to pending mode so wati till it runnig state
                if (!QueryServiceStatus(schService, &ssStatus))
                {
                    // error in getting service status

                    result.status = false;
                    result.code = GetLastError();
                    result.message = GetLastErrorText();
                }
                else
                {
                    // service status retrived successfully
                    // Save the tick count and initial checkpoint.

                    dwStartTickCount = GetTickCount();
                    dwOldCheckPoint = ssStatus.dwCheckPoint;

                    while (ssStatus.dwCurrentState == SERVICE_START_PENDING)

                    {

                        // Just some info...

                        printf("Wait Hint: %d\n", ssStatus.dwWaitHint);

                        // Do not wait longer than the wait hint. A good interval is one tenth the wait hint, but no less than 1 second and no more than 10 seconds...

                        dwWaitTime = ssStatus.dwWaitHint / 10;

                        if (dwWaitTime<1000> 10000)

                            dwWaitTime = 10000;

                        Sleep(dwWaitTime);

                        // Check the status again...

                        if (!QueryServiceStatus(

                                schService, // handle to service

                                &ssStatus)) // address of structure

                            break;

                        if (ssStatus.dwCheckPoint > dwOldCheckPoint)

                        {

                            // The service is making progress...

                            printf("Service starting in progress...\n");

                            dwStartTickCount = GetTickCount();

                            dwOldCheckPoint = ssStatus.dwCheckPoint;
                        }

                        else

                        {

                            if ((GetTickCount() - dwStartTickCount) > ssStatus.dwWaitHint)

                            {

                                // No progress made within the wait hint

                                printf("Well, starting the service looks no progress...\n");

                                break;
                            }
                        }
                    }

                    if (ssStatus.dwCurrentState == SERVICE_RUNNING)

                    {
                        // service start successfuly
                        result.status = true;
                        result.code = 0;
                    }

                    else

                    {

                        // printf("\nService %S not started.\n", lpszServiceName);

                        // printf(" Current State: %d\n", ssStatus.dwCurrentState);

                        // printf(" Exit Code: %d\n", ssStatus.dwWin32ExitCode);

                        // printf(" Service Specific Exit Code: %d\n", ssStatus.dwServiceSpecificExitCode);

                        // printf(" Check Point: %d\n", ssStatus.dwCheckPoint);

                        // printf(" Wait Hint: %d\n", ssStatus.dwWaitHint);
                        result.status = false;
                        result.code = GetLastError();
                        result.message = GetLastErrorText();
                    }
                }
            }

            CloseServiceHandle(schService);
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
}

ResultStruct bind_service(Dart_Port_DL port, LPWSTR serviceName)
{
    struct ResultStruct result;
    _serviceName = serviceName;
    dart_port = port;

    SERVICE_TABLE_ENTRY serviceTable[] = {
        {serviceName, &ServiceMain},
        {NULL, NULL}};

    if (StartServiceCtrlDispatcher(serviceTable))
    {
        result.status = true;
        result.code = 0;
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
    // else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
}

ResultStruct install_service(LPWSTR serviceName, LPWSTR version, LPWSTR serviceDisplayName, LPWSTR appPath, LPWSTR username, LPWSTR password)
{
    struct ResultStruct result;
    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    schSCManager = OpenSCManager(
        NULL,                 // machine (NULL == local)
        NULL,                 // database (NULL == default)
        SC_MANAGER_ALL_ACCESS // access required
    );
    if (schSCManager)
    {
        schService = CreateService(
            schSCManager,                                                // SCManager database
            serviceName,                                                 // name of service
            serviceDisplayName,                                          // name to display
            SERVICE_ALL_ACCESS,                                          // desired access
            SERVICE_WIN32_OWN_PROCESS,                                   // service type
            SERVICE_DEMAND_START,                                        // start type
            SERVICE_ERROR_NORMAL,                                        // error control type
            appPath,                                                     // service's binary
            NULL,                                                        // no load ordering group
            NULL,                                                        // no tag identifier
            L"",                                                         // dependencies
             (username == NULL || username[0] == 0) ? NULL : username, // LocalSystem account
             (password == NULL || password[0] == 0) ? NULL : password  // no password
        );

        if (schService)
        {
            SERVICE_DESCRIPTION sdBuf;
            sdBuf.lpDescription = version;
            if (ChangeServiceConfig2(
                    schService,
                    SERVICE_CONFIG_DESCRIPTION,
                    &sdBuf))
            {
                // service installed
                CloseServiceHandle(schService);
                result.status = true;
                result.code = 0;
            }
            else
            {
                result.status = false;
                result.code = GetLastError();
                result.message = GetLastErrorText();
            }
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }

    return result;
}

ResultStruct remove_service(LPWSTR serviceName)
{
    struct ResultStruct result;
    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    schSCManager = OpenSCManager(
        NULL,                 // machine (NULL == local)
        NULL,                 // database (NULL == default)
        SC_MANAGER_ALL_ACCESS // access required
    );
    if (schSCManager)
    {
        schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);

        if (schService)
        {
            // try to stop the service
            if (ControlService(schService, SERVICE_CONTROL_STOP, &g_ssStatus))
            {
                Sleep(1000);

                while (QueryServiceStatus(schService, &g_ssStatus))
                {
                    if (g_ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
                    {
                        _tprintf_s(TEXT("."));
                        Sleep(1000);
                    }
                    else
                        break;
                }

                if (g_ssStatus.dwCurrentState != SERVICE_STOPPED)
                {
                    result.status = false;
                    result.code = GetLastError();

                    result.message = GetLastErrorText();
                    return result;
                }
            }

            // now remove the service
            if (DeleteService(schService))
            {
                result.status = true;
                result.code = 0;
            }
            else
            {
                result.status = false;
                result.code = GetLastError();
                result.message = GetLastErrorText();
            }

            CloseServiceHandle(schService);

            return result;
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
        return result;
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
}

ResultStruct get_version(LPWSTR serviceName)
{
    struct ResultStruct result;
    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    schSCManager = OpenSCManager(
        NULL,                // machine (NULL == local)
        NULL,                // database (NULL == default)
        SERVICE_QUERY_CONFIG // access required
    );
    if (schSCManager)
    {
        schService = OpenService(schSCManager, serviceName, SERVICE_ALL_ACCESS);

        if (schService)
        {
            DWORD returnLength;
            LPSERVICE_DESCRIPTION descriptionInfo;
            if (!QueryServiceConfig2(schService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &returnLength))
            {

                if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
                {

                    descriptionInfo = (LPSERVICE_DESCRIPTION)LocalAlloc(LMEM_FIXED, returnLength);
                    if (QueryServiceConfig2(
                            schService,
                            SERVICE_CONFIG_DESCRIPTION,
                            (LPBYTE)descriptionInfo,
                            returnLength,
                            &returnLength))
                    {
                        result.status = true;
                        result.code = 0;
                        result.message = descriptionInfo->lpDescription;
                    }
                    else
                    {
                        result.status = false;
                        result.code = GetLastError();
                        result.message = GetLastErrorText();
                    }
                }
                else
                {
                    result.status = false;
                    result.code = GetLastError();
                    result.message = GetLastErrorText();
                }
            }
            else
            {
                result.status = false;
                result.code = GetLastError();
                result.message = GetLastErrorText();
            }
            // descriptionInfo.lpDescription;
            CloseServiceHandle(schService);
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
}

VOID CALLBACK NotifyCallback(PVOID pParameter)
{
    HRESULT hr = S_OK;
    PSERVICE_NOTIFY pNotify = (PSERVICE_NOTIFY)pParameter;
    PNOTIFY_CONTEXT pContext = (PNOTIFY_CONTEXT)pNotify->pContext;

    if (status_change_dart_port != 0)
    {
        ServiceStatusStruct *result = (ServiceStatusStruct *)malloc(sizeof(ServiceStatusStruct));

        result->dwCurrentState = pNotify->ServiceStatus.dwCurrentState;

        Dart_CObject msg;
        msg.type = Dart_CObject_kInt64;

        msg.value.as_int64 = (intptr_t)(result);

        Dart_PostCObject_DL(status_change_dart_port, &msg);
        LPWSTR rrrrr = GetLastErrorText();
        LPWSTR sd = L"";
    }
}

ResultStruct watch_service_status(LPWSTR serviceName)
{

    SC_HANDLE schService;
    SC_HANDLE schSCManager;

    struct ResultStruct result;
    result.status = true;
    result.code = 0;
    result.message = L"Watching ended";

    schSCManager = OpenSCManager(
        NULL,              // machine (NULL == local)
        NULL,              // database (NULL == default)
        SC_MANAGER_CONNECT // access required
    );
    if (schSCManager)
    {
        schService = OpenService(schSCManager, serviceName, SERVICE_QUERY_STATUS);

        if (schService)
        {

            DWORD dwError = ERROR_SUCCESS;
            DWORD dwStatus;
            DWORD dwMask;
            NOTIFY_CONTEXT NotifyContext = {0};
            SERVICE_NOTIFY snServiceNotify;
            // Initialize callback context
            NotifyContext.pszServiceName = serviceName;

            // Intialize notification struct
            snServiceNotify.dwVersion = SERVICE_NOTIFY_STATUS_CHANGE;
            snServiceNotify.pfnNotifyCallback = (PFN_SC_NOTIFY_CALLBACK)NotifyCallback;
            snServiceNotify.pContext = &NotifyContext;

            // We care about changes to RUNNING and STOPPED states only
            dwMask = SERVICE_NOTIFY_RUNNING | SERVICE_NOTIFY_STOPPED | SERVICE_NOTIFY_START_PENDING | SERVICE_NOTIFY_STOP_PENDING;
            // Register for notification
            while (TRUE)
            {
                dwStatus = NotifyServiceStatusChangeW(schService, dwMask, &snServiceNotify);

                if (dwStatus == ERROR_SUCCESS)
                {
                    SleepEx(INFINITE, TRUE);
                }
                else
                {
                    result.status = false;
                    result.code = GetLastError();
                    result.message = GetLastErrorText();
                    dwError = dwStatus;
                    break;
                }
            }

            CloseServiceHandle(schService);
        }
        else
        {
            result.status = false;
            result.code = GetLastError();
            result.message = GetLastErrorText();
        }

        CloseServiceHandle(schSCManager);
    }
    else
    {
        result.status = false;
        result.code = GetLastError();
        result.message = GetLastErrorText();
    }
    return result;
}
void init_service(Dart_Port_DL port)
{
    status_change_dart_port = port;
}
