void setRobotSpeed(int speedParam) {
  speedParam = map(speedParam, 0, 130, 0, 200);
  if (speedParam > 0 || speedParam < 200) {
    robotSpeed = speedParam;
  } else if (speedParam > 200) {
    robotSpeed = 200;
  } else {
    robotSpeed = 0;
  }
  //Serial.printf("Speed set to %d\n", robotSpeed);
  DebugPrint("Speed set to ");
  DebugPrintln(String(robotSpeed));
}

void setTurningAngle(int angle) {
  if (angle < 0 || angle > 100) {
    DebugPrintln("An error occured");
    return;
  }
  double angleDouble = angle;
  double rotateSpeed = angleDouble / 100;
  invertedRotateSpeed = 1.0 - rotateSpeed;
  DebugPrint("InvSpeed:");
  DebugPrintln(String(invertedRotateSpeed));
}

void checkAndSetController(const String sCommand, int clientId) {
  int commands[2];
  if (parseCommand(sCommand, commands) != 0) {
    return;
  }

  int command = commands[0];
  if (command == CONTROL) {
    controller = clientId;
    DebugPrintln("Controller connected.");
  }
}
