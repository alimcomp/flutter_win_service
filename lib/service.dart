import 'dart:ffi';
import 'dart:io';

import 'service_bindings_generated.dart';

startService(String serviceName) => _bindings.startService(serviceName);
installService(String serviceName, String serviceDisplayName, String appPath) =>
    _bindings.installService(serviceName, serviceDisplayName, appPath);

const String _libName = 'service';
Stream<String> get serviceStream => _bindings.serviceStream;

/// The dynamic library in which the symbols for [ServiceBindings] can be found.
final DynamicLibrary _dylib = () {
  if (Platform.isWindows) {
    return DynamicLibrary.open(
        'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\build\\windows\\runner\\Debug\\service.dll');
  }
  throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
}();

/// The bindings to the native functions in [_dylib].
final ServiceBindings _bindings = ServiceBindings(_dylib);
