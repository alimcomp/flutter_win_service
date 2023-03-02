import 'dart:io';
import 'dart:isolate';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:service/service.dart' as service;
import 'package:service/service.dart';
import 'package:path/path.dart' as path;
part 'winows_service_main.dart';


@pragma('vm:entry-point')
void main(List<String> args) async {
  if (args.contains('-s')) {
    await startWindowsService(args);
  } else {
    runApp(const MyApp());
  }
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
  ServiceStatus? serviceStatus;
  @override
  void initState() {
    super.initState();
    service.initService();
    statusSubscription = service.serviceStatusStream.listen((event) {
      setState(() {
        serviceStatus = event;
      });
    });
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
                        if (serviceStatus != null) ...[
                          Text(serviceStatus!.status.toString()),
                          if (serviceStatus!.status == Status.stopPending ||
                              serviceStatus!.status == Status.startPending)
                            const CircularProgressIndicator(),
                        ],
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
              
                              final currentExe = Platform.resolvedExecutable;
                              // String tempPath = tempDir.path;
                              service.installService(serviceName, 'version',
                                  'GposSyncer', "$currentExe -s",
                                  username: "username",
                                  password: "password");
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
                              service.watchService(
                                "GposSyncer",
                              );
                            } catch (e) {
                              print(e);
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('watch service'),
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
                          onPressed: () async {
                            try {
                              await service.reportError("GposSyncer", 200);
                            } catch (e) {
                              ScaffoldMessenger.maybeOf(context)?.showSnackBar(
                                  SnackBar(content: Text(e.toString())));
                            }
                          },
                          child: const Text('stop service'),
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
