import 'dart:math';

class Payload {
  final bool isForward;
  final int left;
  final int right;

  const Payload({
    required this.isForward,
    required this.left,
    required this.right,
  });

  int _mask(int x) => 1 << x;

  // Computes the payload based on the PWM and the speed to send to the
  // web socket.
  int toInteger() {
    int leftWheel = left;
    int rightWheel = right;

    // Set the leftWheel to between 0 and 7.
    if (leftWheel <= -8) leftWheel = -7;
    if (leftWheel < 0) leftWheel = -leftWheel;
    if (rightWheel >= 8) rightWheel = 7;
    if (rightWheel < 0) rightWheel = -rightWheel;

    int payload = leftWheel << 3 | rightWheel;
    if (isForward) payload |= _mask(6);
    return payload;
  }
}

class DirectionManager {
  final double x;
  final double y;
  bool isForward;

  DirectionManager({
    required this.x,
    required this.y,
    this.isForward = false,
  });

  Payload computePayload() {
    // Stop Direction
    if (x == 0 && y == 0) {
      return const Payload(isForward: false, left: 0, right: 0);
    }

    final double angle = atan2(y, x) * (180 / pi);

    // Forward Direction
    if (angle >= 0) {
      return Payload(
        isForward: true,
        left: (angle / 180 * 8).floor(),
        right: ((180 - angle) / 180 * 8).round(),
      );
    }

    return Payload(
      isForward: false,
      left: ((-angle / -180) * 8).floor(),
      right: ((-180 - angle) / -180 * 8).floor(),
    );
  }
}
