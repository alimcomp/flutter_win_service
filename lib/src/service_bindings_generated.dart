// ignore_for_file: always_specify_types
// ignore_for_file: camel_case_types
// ignore_for_file: non_constant_identifier_names

// AUTO GENERATED FILE, DO NOT EDIT.
//
// Generated by `package:ffigen`.
import 'dart:async';
import 'dart:ffi' as ffi;
import 'dart:isolate';

import 'package:ffi/ffi.dart';

import 'result_struct.dart';
import 'service_status.dart';
import 'service_status_struct.dart';

/// Bindings for `src/service.h`.
///
/// Regenerate bindings with `dart run ffigen --config ffigen.yaml`.
///
class ServiceBindings {
  /// Holds the symbol lookup function.
  final ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
      _lookup;

  /// The symbols are looked up in [dynamicLibrary].
  ServiceBindings(ffi.DynamicLibrary dynamicLibrary)
      : _lookup = dynamicLibrary.lookup;

  /// The symbols are looked up with [lookup].
  ServiceBindings.fromLookup(
      ffi.Pointer<T> Function<T extends ffi.NativeType>(String symbolName)
          lookup)
      : _lookup = lookup;

  //DART_EXPORT intptr_t Dart_InitializeApiDL(void* data);
  // Dart_InitializeApiDL defined in Dart SDK (dart_api_dl.c)
  late final _Dart_InitializeApiDLPtr =
      _lookup<ffi.NativeFunction<ffi.IntPtr Function(ffi.Pointer<ffi.Void>)>>(
          "Dart_InitializeApiDL");

  late final _Dart_InitializeApiDL = _Dart_InitializeApiDLPtr.asFunction<
      int Function(ffi.Pointer<ffi.Void>)>();

  ///
  ///service_set_service_status_change_notify
  ///
  late final _initServicePtr =
      _lookup<ffi.NativeFunction<ffi.Void Function(ffi.Int64)>>(
          "service_init_service");
  late final _initService = _initServicePtr.asFunction<void Function(int)>();

  final StreamController<ServiceStatus> _serviceStatusCtrl = StreamController();
  Stream<ServiceStatus> get serviceStatusStream => _serviceStatusCtrl.stream;
  int? startCookie;
  initService() {
    startCookie ??= _Dart_InitializeApiDL(ffi.NativeApi.initializeApiDLData);
    // Call Dart_InitializeApiDL with NativeApi.initializeApiDLData
    final pub = ReceivePort()
      ..listen((message) async {
        final pointer =
            ffi.Pointer<ServiceStatusStruct>.fromAddress(message as int);
        final ref = pointer.ref;
        _serviceStatusCtrl.add(ref.toServiceStatus);
        // calloc.free(pointer);
      });
    _initService(
      pub.sendPort.nativePort,
    );
  }

  ///
  ///service_report_service_error
  ///

  late final _reportErrorPtr = _lookup<
      ffi.NativeFunction<
          ResultStruct Function(
              ffi.Pointer<Utf16>, ffi.Int64)>>("service_report_service_error");
  late final _reportError = _reportErrorPtr
      .asFunction<ResultStruct Function(ffi.Pointer<Utf16>, int)>();
  ResultStruct reportError(String serviceName, int errorCode) =>
      _reportError(serviceName.toNativeUtf16(), errorCode);

  //(LPWSTR serviceName, DWORD errorCode)

  ///
  ///service_watch_service_status
  ///

  late final _watchServicePtr =
      _lookup<ffi.NativeFunction<ResultStruct Function(ffi.Pointer<Utf16>)>>(
          "service_watch_service_status");
  late final _watchService =
      _watchServicePtr.asFunction<ResultStruct Function(ffi.Pointer<Utf16>)>();
  ResultStruct watchService(String serviceName) {
    return _watchService(serviceName.toNativeUtf16());
  }

  ///
  /// get service status
  ///
//
  late final _getServiceStatusPtr = _lookup<
          ffi.NativeFunction<ServiceStatusStruct Function(ffi.Pointer<Utf16>)>>(
      "service_get_service_status");
  late final _getServiceStatus = _getServiceStatusPtr
      .asFunction<ServiceStatusStruct Function(ffi.Pointer<Utf16>)>();

  ServiceStatus getServiceStatus(String serviceName) {
    final result = _getServiceStatus(serviceName.toNativeUtf16());
    if (!result.status) throw result.exception;
    return result.toServiceStatus;
  }

  ///
  ///service_get_version
  ///

  late final _getVersionPtr =
      _lookup<ffi.NativeFunction<ResultStruct Function(ffi.Pointer<Utf16>)>>(
          "service_get_version");
  late final _getVersion =
      _getVersionPtr.asFunction<ResultStruct Function(ffi.Pointer<Utf16>)>();

  String getVersion(String serviceName) {
    final result = _getVersion(serviceName.toNativeUtf16());
    if (!result.status) throw result.exception;
    return result.messageDartString;
  }

  ///
  ///start service function
  ///

  late final _startServicePtr =
      _lookup<ffi.NativeFunction<ResultStruct Function(ffi.Pointer<Utf16>)>>(
          "service_start_service");
  late final _startService =
      _startServicePtr.asFunction<ResultStruct Function(ffi.Pointer<Utf16>)>();
  ResultStruct startService(String serviceName) {
    return _startService(serviceName.toNativeUtf16());
  }

  ///
  /// bind service function
  ///
  late final _bindServicePtr = _lookup<
      ffi.NativeFunction<
          ResultStruct Function(
              ffi.Int64, ffi.Pointer<Utf16>)>>("service_bind_service");
  late final _bindService = _bindServicePtr
      .asFunction<ResultStruct Function(int, ffi.Pointer<Utf16>)>();
  int? cookie;
  final StreamController<ServiceStatus> _servecCtrl = StreamController();
  Stream<ServiceStatus> get serviceStream => _servecCtrl.stream;

  bindService(String serviceName) {
    cookie ??= _Dart_InitializeApiDL(ffi.NativeApi.initializeApiDLData);
    final pub = ReceivePort()
      ..listen((message) async {
        final pointer =
            ffi.Pointer<ServiceStatusStruct>.fromAddress(message as int);

        final ref = pointer.ref;
        _servecCtrl.add(ref.toServiceStatus);
        // calloc.free(pointer);
      });

    // Pass NativePort value (int) to C++ code
    final result =
        _bindService(pub.sendPort.nativePort, serviceName.toNativeUtf16());

    if (!result.status) throw result.exception;
  }

  ///
  /// install service function
  ///wchar_t *version
  late final _installServicePtr = _lookup<
      ffi.NativeFunction<
          ResultStruct Function(
              ffi.Pointer<Utf16>,
              ffi.Pointer<Utf16>,
              ffi.Pointer<Utf16>,
              ffi.Pointer<Utf16>,
              ffi.Pointer<Utf16>,
              ffi.Pointer<Utf16>)>>("service_install_service");
  late final _installService = _installServicePtr.asFunction<
      ResultStruct Function(
          ffi.Pointer<Utf16>,
          ffi.Pointer<Utf16>,
          ffi.Pointer<Utf16>,
          ffi.Pointer<Utf16>,
          ffi.Pointer<Utf16>,
          ffi.Pointer<Utf16>)>();
  installService(String serviceName, String version, String serviceDisplayName,
      String appPath,
      {String username ='', String password=''}) {
    final result = _installService(
      serviceName.toNativeUtf16(),
      version.toNativeUtf16(),
      serviceDisplayName.toNativeUtf16(),
      appPath.toNativeUtf16(),
      username.toNativeUtf16(),
      password.toNativeUtf16(),
    );
    if (!result.status) throw result.exception;
  }

  ///
  ///service_remove_service
  ///
  late final _removeServicePtr =
      _lookup<ffi.NativeFunction<ResultStruct Function(ffi.Pointer<Utf16>)>>(
          "service_remove_service");
  late final _removeService =
      _removeServicePtr.asFunction<ResultStruct Function(ffi.Pointer<Utf16>)>();

  removeService(String serviceName) {
    final result = _removeService(serviceName.toNativeUtf16());

    if (!result.status) throw result.exception;
  }
}
