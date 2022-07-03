
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <tchar.h>
#include "include/result_struct.h"
#include "include/win_service.h"
//
//#include "service.h"

// Main function to be executed as entire service code.
// Handler for service control events.

//////////////////////////////////////////////////////////////////////////////

SERVICE_STATUS_HANDLE g_ServiceStatusHandle;
SERVICE_STATUS g_ssStatus;
HANDLE g_StopEvent;
DWORD g_CurrentState = 0;
DWORD g_SystemShutdown = 0;
static Dart_Port_DL dart_port = 0;
LPWSTR _serviceName = L"";

////
////  FUNCTION: GetLastErrorText
////
////  PURPOSE: copies error message text to string
////
////  PARAMETERS:
////    lpszBuf - destination buffer
////    dwSize - size of buffer
////
////  RETURN VALUE:
////    destination buffer
////
////  COMMENTS:
////
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
        0,
        ERROR_SERVICE_SPECIFIC_ERROR,
        errorCode,
        0,
        0,
    };
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
    Beep(523, 500);
    // Must be called at start.

    g_ServiceStatusHandle = RegisterServiceCtrlHandlerEx(_serviceName, &HandlerEx, NULL);

    // Startup code.
    ReportStatus(SERVICE_START_PENDING);
    g_StopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    /* Here initialize service...
    Load configuration, acquire resources etc. */
    ReportStatus(SERVICE_RUNNING);
    // This sample service does "BEEP!" every 3 seconds.

    /* Main service code
    *
    Loop, do some work, block if nothing to do,
    wait or poll for g_StopEvent... */
    // FILE *fp;
    // fopen_s(&fp, "C:\\my_file.txt", "w+");
    while (WaitForSingleObject(g_StopEvent, 500) != WAIT_OBJECT_0)
    {

        // fputs("Start.\n", fp);
        if (dart_port == 0)
            return;
        // fputs("dart port is ok.\n", fp);
        Dart_CObject msg;
        msg.type = Dart_CObject_kString;
        msg.value.as_string = (char *)" hello from c to dart";
        // The function is thread-safe; you can call it anywhere on your C++ code
        Dart_PostCObject_DL(dart_port, &msg);
        // DWORD error = GetLastError();
        // if (error != 0)
        // {
        //     // wchar_t err[256];
        //     // // memset(err, 0, 256);
        //     // FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
        //     //                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);

        //     // fprintf(fp, "%ls", err);
        //     // //                 fprintf(fp, "This is testing for fprintf...\n");
        // }

        // fprintf(fp, message);
    }
    // fputs("service stoped.\n", fp);
    // fclose(fp);
    ReportStatus(SERVICE_STOP_PENDING);
    /* Here finalize service...
    Save all unsaved data etc., but do it quickly.
    If g_SystemShutdown, you can skip freeing memory etc. */
    CloseHandle(g_StopEvent);
    ReportStatus(SERVICE_STOPPED);
}
// Create a string with last error message

ResultStruct start_service(Dart_Port_DL port, LPWSTR serviceName)
{
    struct ResultStruct result;
    _serviceName = serviceName;
    dart_port = port;

    Beep(523, 500);

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
        // return GetLastError();
    }
    return result;
    // else if (GetLastError() == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT)
    //     return -1; // Program not started as a service.
    // else
    //     return -2; // Other error.
}

ResultStruct install_service(LPWSTR serviceName, LPWSTR serviceDisplayName, LPWSTR appPath)
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
            schSCManager,              // SCManager database
            serviceName,               // name of service
            serviceDisplayName,        // name to display
            SERVICE_ALL_ACCESS,        // desired access
            SERVICE_WIN32_OWN_PROCESS, // service type
            SERVICE_DEMAND_START,      // start type
            SERVICE_ERROR_NORMAL,      // error control type
            appPath,                   // service's binary
            NULL,                      // no load ordering group
            NULL,                      // no tag identifier
            L"",                       // dependencies
            NULL,                      // LocalSystem account
            NULL);                     // no password

        if (schService)
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
//
//
//
//
/////////////////////////////////////////////////////////////////////
////
////  The following code is for running the service as a console app
////
//
//
////
////  FUNCTION: CmdDebugService(int argc, char ** argv)
////
////  PURPOSE: Runs the service as a console application
////
////  PARAMETERS:
////    argc - number of command line arguments
////    argv - array of command line arguments
////
////  RETURN VALUE:
////    none
////
////  COMMENTS:
////
// void CmdDebugService(int argc, char** argv)
//{
//     DWORD dwArgc;
//     LPTSTR* lpszArgv;
//
//#ifdef UNICODE
//     lpszArgv = CommandLineToArgvW(GetCommandLineW(), &(dwArgc));
//#else
//     dwArgc = (DWORD)argc;
//     lpszArgv = argv;
//#endif
//
//     _tprintf_s(TEXT("Debugging %s.\n"), TEXT(SZSERVICEDISPLAYNAME));
//
//     SetConsoleCtrlHandler(ControlHandler, TRUE);
//
//     ServiceStart(dwArgc, lpszArgv);
// }
//
//
////
////  FUNCTION: ControlHandler ( DWORD dwCtrlType )
////
////  PURPOSE: Handled console control events
////
////  PARAMETERS:
////    dwCtrlType - type of control event
////
////  RETURN VALUE:
////    True - handled
////    False - unhandled
////
////  COMMENTS:
////
// BOOL WINAPI ControlHandler(DWORD dwCtrlType)
//{
//     switch (dwCtrlType)
//     {
//     case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
//     case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
//         _tprintf_s(TEXT("Stopping %s.\n"), TEXT(SZSERVICEDISPLAYNAME));
//         ServiceStop();
//         return TRUE;
//         break;
//
//     }
//     return FALSE;
// }
