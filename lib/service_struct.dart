import 'dart:ffi';
import 'package:ffi/ffi.dart';

class StatusStruct extends Struct {
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
}
