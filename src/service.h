#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/result_struct.h"
#include "include/win_service.h"
#include "include/dart_api_dl.h"

#if _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#if _WIN32
#define FFI_PLUGIN_EXPORT __declspec(dllexport)
#else
#define FFI_PLUGIN_EXPORT
#endif

FFI_PLUGIN_EXPORT ResultStruct service_start_service(Dart_Port_DL port, wchar_t* serviceName);

FFI_PLUGIN_EXPORT ResultStruct service_install_service(wchar_t* serviceName, wchar_t* serviceDisplayName, wchar_t* appPath);
FFI_PLUGIN_EXPORT ResultStruct service_remove_service(wchar_t* serviceName);