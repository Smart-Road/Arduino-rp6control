void useCommand(const String sCommand) 
{
  long commands[2];
  if (!parseCommand(sCommand, commands)) {
    DEBUGCODE(Serial.println("parsing command " + sCommand + " failed"));
    return;
  }

  long command = commands[0];
  long parameter = commands[1];
  switch (command) {
    case SPEED:
      setRobotSpeed(parameter);
      break;
    case ANGLE:
      setTurningAngle(parameter);
      break;
    case SERVERIP:
      parseAndSetServerIp(getParamString(sCommand));
      break;
    case MAXSPEED:
      setMaxSpeed(parameter);
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
      DEBUGCODE(Serial.println("An error occured"));
      break;
  }
}

