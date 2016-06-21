#define SPEED 1
#define ANGLE 2
#define DIRECTION 3
#define CONTROL 4

#define RIGHT 10
#define LEFT 11
#define FORWARD 12
#define BACKWARD 13

void useCommand(const String sCommand) {
  int commands[2];
  if (parseCommand(sCommand, commands) != 0) {
    return;
  }

  int command = commands[0];
  int parameter = commands[1];
  switch (command) {
    case SPEED:
      setRobotSpeed(parameter);
      break;
    case ANGLE:
      setTurningAngle(parameter);
    case DIRECTION:
      switch (parameter) {
        case LEFT:
          state = State::Left;
          break;
        case RIGHT:
          state = State::Right;
          break;
        case FORWARD:
          Rp6.changeDirection(RP6_FORWARD);
          state = State::Forward;
          break;
        case BACKWARD:
          Rp6.changeDirection(RP6_BACKWARD);
          state = State::Backward;
          break;
      }
      break;
    default:
      DebugPrintln("An error occured");
      break;
  }
}

int parseCommand(const String command, int commands[2]) { // commands has to be of sizeof(int) * 2
  if (command == NULL) {
    return -1;
  }
  int paramcheck = command.indexOf(":");
  if (paramcheck < 0) { // if no parameter is given
    DebugPrintln("ERROR:Command without parameter was given to parser");
    return -1;
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
  } else {

    return -1;
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
      return -1;
    }
  } else if (commands[0] == SPEED || commands[0] == ANGLE) {
    commands[1] = parameter.toInt();
  } else if (commands[0] == CONTROL) {
    commands[1] = 666; // doesn't really matter what this will be
  } else {
    return -1;
  }

  return 0;
}

