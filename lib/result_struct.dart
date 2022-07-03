import 'dart:ffi';
import 'package:ffi/ffi.dart';

import 'service_exception.dart';

class ResultStruct extends Struct {
  @Bool()
  external bool status;
  @Int64()
  external int code;
  external Pointer<Utf16> message;
  String get messageDartString => message.toDartString();

  ServiceException get exception => ServiceException(code, messageDartString);
}


    // bool status;
    // wchar_t *message;