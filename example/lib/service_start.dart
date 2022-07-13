import 'dart:async';
import 'dart:io';
import 'dart:isolate';

import 'package:service/service.dart' as service;
import 'package:service/service_status.dart';

void main(List<String> args) async {
  var serviceName = "GposSyncer";
  service.bindService(serviceName);
  Isolate? test;

  await for (var event in service.serviceStream) {
    if (event.status == Status.running) {
      Isolate.spawn(startTask, "").then((value) => test = value);
    } else if (event.status == Status.stopped) {
      test?.kill();
    }
  }
}

startTask(String message)  {
  final testf = File(
      'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\t2541.txt');


  Timer.periodic(Duration(seconds: 1), (timer) async {
    testf.writeAsString("${timer.tick}");
    if (timer.tick == 100) {
      testf.writeAsString("end meeted");
      service.reportError("GposSyncer", 254);
    }
  });
}
