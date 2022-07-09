import 'dart:io';

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
  late StreamSubscription statusSubscription;
  @override
  void initState() {
    super.initState();
    // service.initService("GposSyncer");
    // statusSubscription = service.notificationStream.listen((event) {
    //   print(event);
    // });
  }

  @override
  void dispose() {
    statusSubscription.cancel();
    super.dispose();
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
                              final version = service.getVersion(
                                "GposSyncer",
                              );

                              print(version);
                            } catch (e) {
                              print(e);
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('get version '),
                        ),
                        OutlinedButton(
                          onPressed: () {
                            try {
                              //  String dir = Directory.current.path;
                              final dir = Platform.resolvedExecutable;
                              final lastIndex = dir.lastIndexOf('\\');
                              final basePath = dir.substring(0, lastIndex);

                              service.installService(
                                "GposSyncer",
                                "1.1.0.1",
                                "GPOS SYNC",
                                "C:\\Users\\mohammadi\\Desktop\\service\\service\\example\\lib\\service_start.exe",
                              );
                            } catch (e) {
                              print(e);
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('install service'),
                        ),
                        OutlinedButton(
                          onPressed: () async {
                            try {
                              final serviceStats =
                                  service.getServiceStatus("GposSyncer");
                              print(serviceStats.status);
                            } catch (e) {
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('Get service status'),
                        ),
                        OutlinedButton(
                          onPressed: () {
                            try {
                              // service.initService(
                              //   "GposSyncer",
                              // );
                            } catch (e) {
                              print(e);
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('init service'),
                        ),
                        OutlinedButton(
                          onPressed: () async {
                            try {
                              await service.startService("GposSyncer");
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
                              service.removeService("GposSyncer");
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
