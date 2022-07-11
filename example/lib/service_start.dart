import 'dart:io';

import 'package:service/service.dart' as service;
import 'package:service/service_status.dart';

void main(List<String> args) {
  File file = File(
      'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\test.txt');
  // file.writeAsStringSync("hello\n", mode: FileMode.append);
  service.serviceStream.listen((event) async {
    File file = File(
        'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\test.txt');
    file.writeAsStringSync("${event.status}\n", mode: FileMode.append);
    if (event.status == Status.running) {
      file.writeAsStringSync("service start on background ",
          mode: FileMode.append);
      await Future.delayed(Duration(seconds: 20));
      throw Exception('that s error ');
    }
  });

  service.bindService("GposSyncer");
}
