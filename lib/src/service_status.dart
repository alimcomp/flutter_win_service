class ServiceStatus {
  final Status status;

  ServiceStatus(this.status);
}

enum Status {
  stopped(1),
  startPending(2),
  stopPending(3),
  running(4),
  continuePending(5),
  pausePending(6),
  paused(7),
  unknown(8);

  const Status(this.value);
  final int value;

  static Status fromInt(int value) => $StatusMap[value] ?? unknown;
}

const Map<int, Status> $StatusMap = {
  1: Status.stopped,
  2: Status.startPending,
  3: Status.stopPending,
  4: Status.running,
  5: Status.continuePending,
  6: Status.pausePending,
  7: Status.paused,
};
