bool parseCommand(const String command, long commands[2]) // commands has to be of sizeof(long) * 2
{
  if (command == NULL) {
    return false;
  }
  int paramcheck = command.indexOf(":");
  if (paramcheck < 0) { // if no parameter is given
    DEBUGCODE(Serial.print("ERROR:Command without parameter was given to parser:"));
    DEBUGCODE(Serial.println(command));
    return false;
  }

  // parse command
  int endCommand = command.indexOf(":");
  String beginCommand = command.substring(0, endCommand);
  if (beginCommand == "SPEED") {
    commands[0] = SPEED;
  } else if (beginCommand == "ANGLE") {
    commands[0] = ANGLE;
  } else if (beginCommand == "DIRECTION") {
    commands[0] = DIRECTION;
  } else if (beginCommand == "CONTROL") {
    commands[0] = CONTROL;
  } else if (beginCommand == "SERVERIP") {
    commands[0] = SERVERIP;
  } else if (beginCommand == "RFID") {
    commands[0] = RFID;
  } else {
    return false;
  }

  // parse parameter
  String parameter = command.substring(endCommand + 1);
  if (commands[0] == DIRECTION) {
    if (parameter == "LEFT") {
      commands[1] = LEFT;
    } else if (parameter == "RIGHT") {
      commands[1] = RIGHT;
    } else if (parameter == "FORWARD") {
      commands[1] = FORWARD;
    } else if (parameter == "BACKWARD") {
      commands[1] = BACKWARD;
    } else {
      return false;
    }
  } else if (commands[0] == SPEED ||
             commands[0] == ANGLE) {
    commands[1] = parameter.toInt();
  } else if (commands[0] == CONTROL ||
             commands[0] == SERVERIP ||
             commands[0] == RFID) {
    // command is valid, second arg is a string. use getparamstring
    // commands[1] is undefined
  } else {
    DEBUGCODE(Serial.printf("command has value of:%ld, which is unknown.\n", commands[0]));
    return false;
  }

  return true;
}

String getParamString(const String command)
{
  int endCommand = command.indexOf(":");
  if (endCommand < 0 || endCommand == command.length() - 1) {
    DEBUGCODE(Serial.println("Error occured in getParamString, no parameter found"));
    return "";
  }
  String parameter = command.substring(endCommand + 1);
  return parameter;
}
