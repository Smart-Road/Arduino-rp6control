#include <stdio.h>

const int maxRobotSpeed = 200;
const int minRobotSpeed = 0;

void setRobotSpeed(int speedParam) {
  speedParam = map(speedParam, 0, 130, minRobotSpeed, maxRobotSpeed);
  if (speedParam >= minRobotSpeed && speedParam <= maxRobotSpeed) {
    robotSpeed = speedParam;
  } else if (speedParam > maxRobotSpeed) {
    robotSpeed = maxRobotSpeed;
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
  DebugPrint("Inverted angle:");
  DebugPrintln(String(invertedRotateSpeed));
}

bool checkAndSetController(const String sCommand, int clientId) {
  int commands[2];
  if (!parseCommand(sCommand, commands)) {
    return false;
  }

  int command = commands[0];
  if (command == CONTROL) {
    controller = clientId;
    String sController = getParamString(sCommand);
    DebugPrint("Controller (");
    DebugPrint(sController);
    DebugPrintln(") connected.");
    return true;
  }
  return false;
}

void parseAndSetServerIp(String sIp) {
  // parse ip, put it in ipPieces
  const int ipPiecesSize = 4;
  int ipPieces[ipPiecesSize];
  int ipPieceIndex = 0;
  String s = "";
  for (size_t i = 0; i < sIp.length(); i++) {
    char readChar = sIp.charAt(i);
    if (readChar != '.') {
      s += readChar;
    } else {
      ipPieces[ipPieceIndex] = s.toInt();
      s = "";
      ipPieceIndex++;
    }
  }
  ipPieces[3] = s.toInt();

  // debug print all values of ipPieces
  DebugPrint("Ip address values:");
  for (int i = 0; i < ipPiecesSize; i++) {
    DebugPrint(String(ipPieces[i]));
    DebugPrint(" ");
  }
  DebugPrintln("");
  
  // set ip to the right ip address
  pcClientIp = IPAddress(ipPieces[0], ipPieces[1], ipPieces[2], ipPieces[3]);
}

