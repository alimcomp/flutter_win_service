import 'dart:async';
import 'dart:io';
import 'dart:isolate';

import 'package:win_service/win_service.dart' as service;
import 'package:win_service/win_service.dart';

void main(List<String> args) async {
  var serviceName = "GposSyncer";
  Isolate? test;

  service.initService();
  service.watchService(serviceName);
  service.serviceStatusStream.listen((event) async {
    if (event.status == Status.running) {
      test = await Isolate.spawn(startBackGroundTask, "");
    } else {
      test?.kill();
    }
  });

  await Isolate.spawn(startTask, "");
}

Future<void> startTask(String message) async {
  service.bindService("GposSyncer");
}

Future<void> startBackGroundTask(String message) async {
  final testf = File(
      'C:\\Users\\mohammadi\\Desktop\\My files\\service\\service\\example\\lib\\t2.txt');
  var timer = Timer.periodic(const Duration(seconds: 2), (timer) {
    testf.writeAsStringSync("\n${timer.tick}", mode: FileMode.append);
    if (timer.tick == 100) {
      testf.writeAsStringSync("error happend", mode: FileMode.append);
      service.reportError("GposSyncer", 500);
    }
  });
}
