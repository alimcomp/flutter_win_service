part of 'main.dart';

Future<void> startBackGroundTask(String message) async {
  isRunnig = Completer();
  final currentExe = Platform.resolvedExecutable;
  
  final testf = File(path.join(path.dirname(currentExe), 't2.txt'));

  var timer = Timer.periodic(const Duration(seconds: 2), (timer) {
    testf.writeAsStringSync("\n${timer.tick}", mode: FileMode.append);
    if (timer.tick == 100) {
      testf.writeAsStringSync("error happend", mode: FileMode.append);
      service.reportError("GposSyncer", 500);
    }
  });
  isRunnig?.complete(true);
}

 Completer<bool>? isRunnig;
var serviceName = "GposSyncer";
Future<void> initWindowsBackgroundService() async {
  WidgetsFlutterBinding.ensureInitialized();

  service.initService();
  service.watchService(serviceName);
  service.serviceStatusStream.listen((event) async {
    if (event.status == Status.running) {
      if (isRunnig ==null || !(await isRunnig!.future)) {
        await startBackGroundTask("");
      }
    } else if (event.status == Status.stopped) {}
  });
  await Isolate.spawn(startTask, "");
}

//windows

Future<void> startTask(String message) async {

  service.bindService(serviceName);
}

Future<void> startWindowsService(List<String> args) async {
  //start just as service
  WidgetsFlutterBinding.ensureInitialized();
  await initWindowsBackgroundService();
  runApp(
    MaterialApp(
      home: Builder(builder: (context) {
        return Container(
          child: Center(
            child: Text('سرویس همگام سازی لطفا به صورت دستی اجرا نکنید'),
          ),
        );
      }),
    ),
  );
}
