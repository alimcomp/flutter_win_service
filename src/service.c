
#include "service.h"
#include "include/result_struct.h"
#include "include/win_service.h"

FFI_PLUGIN_EXPORT ResultStruct service_start_service(Dart_Port_DL port, wchar_t *serviceName)
{
  // Dart_CObject msg;
  //   msg.type = Dart_CObject_kString;
  //   msg.value.as_string = (char *)" hello from c to dart";

  //   // The function is thread-safe; you can call it anywhere on your C++ code
  //   Dart_PostCObject_DL(port, &msg);
  return start_service(port, serviceName);
}

FFI_PLUGIN_EXPORT ResultStruct service_install_service(wchar_t *serviceName, wchar_t *serviceDisplayName, wchar_t *appPath)
{

  return install_service(serviceName, serviceDisplayName, appPath);
}
FFI_PLUGIN_EXPORT ResultStruct service_remove_service(wchar_t *serviceName)
{
  return remove_service(serviceName);
}