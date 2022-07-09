import 'dart:io';

import 'package:service/service.dart' as service;

void main(List<String> args) {
  File file = File(
      'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\test.txt');
  // file.writeAsStringSync("hello\n", mode: FileMode.append);
  service.serviceStream.listen((event) {
    File file = File(
        'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\test.txt');
    file.writeAsStringSync("${event.status}\n", mode: FileMode.append);
    // if (event.status == Status.running) {
    //   file.writeAsStringSync("service start on background ",
    //       mode: FileMode.append);
    // }
  });

  service.bindService("GposSyncer");
}
