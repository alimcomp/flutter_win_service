// #define SERVICE_STOPPED                        0x00000001
// #define SERVICE_START_PENDING                  0x00000002
// #define SERVICE_STOP_PENDING                   0x00000003
// #define SERVICE_RUNNING                        0x00000004
// #define SERVICE_CONTINUE_PENDING               0x00000005
// #define SERVICE_PAUSE_PENDING                  0x00000006
// #define SERVICE_PAUSED                         0x00000007

// typedef struct ServiceStatusStruct
// {
//     bool status;
//     size_t code;
//     wchar_t *message;
//     size_t dwServiceType;
//     size_t dwCurrentState;
//     size_t dwControlsAccepted;
//     size_t dwWin32ExitCode;
//     size_t dwServiceSpecificExitCode;
//     size_t dwCheckPoint;
//     size_t dwWaitHint;
// } ServiceStatusStruct;

import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:service/service_status.dart';

import 'service_exception.dart';

class ServiceStatusStruct extends Struct {
  @Bool()
  external bool status;
  @Int64()
  external int code;
  external Pointer<Utf16> message;
  String get messageDartString => message.toDartString();

  @Int64()
  external int dwServiceType;
  @Int64()
  external int dwCurrentState;
  @Int64()
  external int dwControlsAccepted;
  @Int64()
  external int dwWin32ExitCode;
  @Int64()
  external int dwServiceSpecificExitCode;
  @Int64()
  external int dwCheckPoint;
  @Int64()
  external int dwWaitHint;

  ServiceException get exception => ServiceException(code, messageDartString);

  ServiceStatus get toServiceStatus {
    return ServiceStatus(Status.fromInt(dwCurrentState));
  }
}
