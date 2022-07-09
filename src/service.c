
#include "service.h"
#include "include/result_struct.h"
#include "include/win_service.h"
// FFI_PLUGIN_EXPORT ResultStruct service_init_service(Dart_Port_DL port, wchar_t *serviceName)
// {
//   return  init_service(port, serviceName);
// }

FFI_PLUGIN_EXPORT ServiceStatusStruct service_get_service_status(wchar_t *serviceName)
{
  return get_service_status(serviceName);
}

FFI_PLUGIN_EXPORT ResultStruct service_get_version(wchar_t *serviceName)
{
  return get_version(serviceName);
}

FFI_PLUGIN_EXPORT ResultStruct service_start_service(wchar_t *serviceName)
{
  return start_service(serviceName);
}

FFI_PLUGIN_EXPORT ResultStruct service_bind_service(Dart_Port_DL port, wchar_t *serviceName)
{
  return bind_service(port, serviceName);
}

FFI_PLUGIN_EXPORT ResultStruct service_install_service(wchar_t *serviceName, wchar_t *version, wchar_t *serviceDisplayName, wchar_t *appPath)
{

  return install_service(serviceName, version, serviceDisplayName, appPath);
}
FFI_PLUGIN_EXPORT ResultStruct service_remove_service(wchar_t *serviceName)
{
  return remove_service(serviceName);
}