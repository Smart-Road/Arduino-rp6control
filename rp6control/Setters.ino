void setRobotSpeed(int speedParam) {
  if (speedParam >= 0 && speedParam <= maxSpeed) {
    cruiseSpeed = speedParam;
  } else if (speedParam > maxSpeed) {
    DEBUGCODE(Serial.println("Speed higher than maxSpeed was tried to be set, reverting to maxSpeed"));
    cruiseSpeed = maxSpeed;
  } else {
    cruiseSpeed = 0;
  }
  DEBUGCODE(Serial.printf("Speed set to %d\n", cruiseSpeed));
}

void setTurningAngle(int angle) {
  if (angle < 0 || angle > 100) {
    DEBUGCODE(Serial.println("An error occured in setTurningAngle"));
    return;
  }
  float angleFloat = angle;
  float rotateSpeed = angleFloat / 100;
  invertedRotateSpeed = 1.0 - rotateSpeed;
  DEBUGCODE(Serial.println("Inverted angle:" + String(invertedRotateSpeed)));
}

bool checkAndSetController(const String sCommand, int clientId) {
  long commands[2];
  if (!parseCommand(sCommand, commands)) {
    return false;
  }

  int command = commands[0];
  if (command == CONTROL) {
    controller = clientId;
    String sController = getParamString(sCommand);
    DEBUGCODE(Serial.println("Controller(" + sController + ") connected."));
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
  DEBUGCODE(Serial.print("Ip address values:"));
  for (int i = 0; i < ipPiecesSize; i++) {
    DEBUGCODE(Serial.print(String(ipPieces[i]) + " "));
  }
  DEBUGCODE(Serial.println());

  // set ip to the right ip address
  pcClientIp = IPAddress(ipPieces[0], ipPieces[1], ipPieces[2], ipPieces[3]);
}

void setMaxSpeed(int newMaxSpeed) {
  if (newMaxSpeed < MAXSPEEDMIN || newMaxSpeed > MAXSPEEDMAX) {
    DEBUGCODE(Serial.printf("Invalid max speed received:%d", newMaxSpeed));
    return;
  }
  maxSpeed = newMaxSpeed;
  setRobotSpeed(cruiseSpeed); // set robot speed again, so if the speed was too high, the speed will be lowered to the right max speed
  
  if (controller != notInitializedController) {
    SendMessage(client[controller], "MAXSPEED:" + String(newMaxSpeed));
  }
}

