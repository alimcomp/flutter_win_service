
#include "dart_api_dl.h"
#include "result_struct.h"
#include <windows.h>
ResultStruct watch_service_status(LPWSTR serviceName);
void init_service(Dart_Port_DL port);
ServiceStatusStruct get_service_status(LPWSTR serviceName);
ResultStruct get_version(LPWSTR serviceName);
ResultStruct report_service_error(LPWSTR serviceName, DWORD errorCode);
ResultStruct start_service(LPWSTR serviceName);
ResultStruct bind_service(Dart_Port_DL port,LPWSTR serviceName);
ResultStruct install_service(LPWSTR serviceName,LPWSTR version,LPWSTR serviceDisplayName,LPWSTR appPath);
ResultStruct remove_service(LPWSTR serviceName);

// LPTSTR GetLastErrorText(LPTSTR lpszBuf, DWORD dwSize)
// {
//    DWORD dwRet;
//    LPTSTR lpszTemp = NULL;

//    dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
//        NULL,
//        GetLastError(),
//        LANG_NEUTRAL,
//        (LPTSTR)&lpszTemp,
//        0,
//        NULL);

//    // supplied buffer is not long enough
//    if (!dwRet || ((long)dwSize < (long)dwRet + 14))
//        lpszBuf[0] = TEXT('\0');
//    else
//    {
//        lpszTemp[lstrlen(lpszTemp) - 2] = TEXT('\0');  //remove cr and newline character
//        _stprintf_s(lpszBuf, GetLastError(), TEXT("%s (0x%x)"), lpszTemp);
//    }

//    if (lpszTemp)
//        LocalFree((HLOCAL)lpszTemp);

//    return lpszBuf;
// }