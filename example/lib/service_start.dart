import 'package:service/service.dart' as service;
import 'dart:convert';
import 'dart:io';

void main(List<String> args) {
  final test = service.initService("SimpleService");
  File file = File(
      'C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\test.txt');
  file.writeAsStringSync("$test\n", mode: FileMode.append);
  service.serviceStream.listen((event) {
    file.writeAsStringSync("$event\n", mode: FileMode.append);
  });
}
