import 'package:flutter/material.dart';
import 'dart:async';

import 'package:service/service.dart' as service;

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({Key? key}) : super(key: key);

  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  late int sumResult;
  late Future<int> sumAsyncResult;

  @override
  void initState() {
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    const textStyle = TextStyle(fontSize: 25);
    const spacerSmall = SizedBox(height: 10);
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Native Packages'),
        ),
        body: Builder(builder: (context) {
          return SingleChildScrollView(
            child: Container(
              padding: const EdgeInsets.all(10),
              child: Column(
                children: [
                  Center(
                    child: Column(
                      children: [
                        OutlinedButton(
                          onPressed: () {
                            try {
                              service.installService(
                                "SimpleService",
                                "Simple service display name",
                                "C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\service_start.exe",
                              );
                            } catch (e) {
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('install service'),
                        ),
                        OutlinedButton(
                          onPressed: () {
                            try {
                              service.startService(
                                "SimpleService",
                              );
                            } catch (e) {
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('start service'),
                        ),
                        OutlinedButton(
                          onPressed: () {
                            try {
                              service.startService(
                                "SimpleService",
                              );
                            } catch (e) {
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('remove service'),
                        ),
                      ],
                    ),
                  ),
                ],
              ),
            ),
          );
        }),
      ),
    );
  }
}
