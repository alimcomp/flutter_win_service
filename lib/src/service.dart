import 'dart:async';
import 'dart:ffi';
import 'dart:io';
import 'dart:isolate';

import 'package:path/path.dart' as path;

import 'result_struct.dart';
import 'service_bindings_generated.dart';
import 'service_status.dart';

Stream<ServiceStatus> get serviceStatusStream => _bindings.serviceStatusStream;

void initService() => _bindings.initService();

Future<void> reportError(String serviceName, int errorCode) async {
  /// Where I listen to the message from startServiceIsolate
  Completer<void> resultComplater = Completer();
  final mainReceivePort = ReceivePort();
  Isolate? test;

  mainReceivePort.listen((message) {
    if (message is SendPort) {
      message.send(serviceName);
    } else if (message is ResultStruct) {
      if (!message.status) {
        resultComplater.completeError(message.exception);
      } else {
        resultComplater.complete();
      }
      test?.kill();
      // todo think about canclation
    }
  });

  test = await Isolate.spawn((SendPort sendPort) async {
    final ReceivePort reportErrorIsolatePort = ReceivePort();
    reportErrorIsolatePort.listen((message) {
      if (message is String) {
        final result = _bindings.reportError(message, 0);
        sendPort.send(result);
      }
    });
    sendPort.send(reportErrorIsolatePort.sendPort);
  }, mainReceivePort.sendPort);

  return resultComplater.future;
}

Future<void> watchService(String serviceName) async {
  /// Where I listen to the message from startServiceIsolate
  Completer<void> resultComplater = Completer();
  final mainReceivePort = ReceivePort();
  Isolate? test;

  mainReceivePort.listen((message) {
    if (message is SendPort) {
      message.send(serviceName);
    } else if (message is ResultStruct) {
      if (!message.status) {
        resultComplater.completeError(message.exception);
      } else {
        resultComplater.complete();
      }
      test?.kill();
      // todo think about canclation
    }
  });

  test = await Isolate.spawn((SendPort sendPort) async {
    final ReceivePort watchServiceIsolatePort = ReceivePort();
    watchServiceIsolatePort.listen((message) {
      if (message is String) {
        final result = _bindings.watchService(message);
        sendPort.send(result);
      }
    });
    sendPort.send(watchServiceIsolatePort.sendPort);
  }, mainReceivePort.sendPort);

  return resultComplater.future;
}

Future<void> startService(String serviceName) async {
  /// Where I listen to the message from startServiceIsolate
  Completer<void> resultComplater = Completer();
  final mainReceivePort = ReceivePort();
  Isolate? test;

  mainReceivePort.listen((message) {
    if (message is SendPort) {
      message.send(serviceName);
    } else if (message is ResultStruct) {
      if (!message.status) {
        resultComplater.completeError(message.exception);
      } else {
        resultComplater.complete();
      }
      test?.kill();
      // todo think about canclation
    }
  });

  test = await Isolate.spawn((SendPort sendPort) async {
    final ReceivePort startServiceIsolatePort = ReceivePort();
    startServiceIsolatePort.listen((message) {
      if (message is String) {
        final result = _bindings.startService(message);
        sendPort.send(result);
      }
    });
    sendPort.send(startServiceIsolatePort.sendPort);
  }, mainReceivePort.sendPort);

  return resultComplater.future;
}

ServiceStatus getServiceStatus(String serviceName) =>
    _bindings.getServiceStatus(serviceName);

String getVersion(String serviceName) => _bindings.getVersion(serviceName);
//startService(String serviceName) => _bindings.startService(serviceName);
removeService(String serviceName) => _bindings.removeService(serviceName);
bindService(String serviceName) => _bindings.bindService(serviceName);
installService(String serviceName, String version, String serviceDisplayName,
        String appPath) =>
    _bindings.installService(serviceName, version, serviceDisplayName, appPath);

const String _libName = 'service';
Stream<ServiceStatus> get serviceStream => _bindings.serviceStream;

/// The dynamic library in which the symbols for [ServiceBindings] can be found.
final DynamicLibrary _dylib = () {
  const bool kReleaseMode = bool.fromEnvironment('dart.vm.product');
  if (Platform.isWindows) {
    if (kReleaseMode) {
      return DynamicLibrary.open('$_libName.dll');
    } else {
      final currentExe = Platform.resolvedExecutable;
      final basePath = path.basename(currentExe);
      final libPath = path.join(basePath, _libName);
      return DynamicLibrary.open(libPath);
    }
  }
  throw UnsupportedError('Unknown platform: ${Platform.operatingSystem}');
}();

/// The bindings to the native functions in [_dylib].
final ServiceBindings _bindings = ServiceBindings(_dylib);
