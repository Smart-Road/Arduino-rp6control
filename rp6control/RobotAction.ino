const int maxRobotSpeed = 200;
const int minRobotSpeed = 0;

void robotAction() 
{
  int mappedSpeed = map(cruiseSpeed, 0, MAXSPEEDMAX, minRobotSpeed, maxRobotSpeed);
  switch (state) {
    case State::Forward:
    case State::Backward:
      // keep it at robotSpeed
      Rp6.moveAtSpeed(mappedSpeed, mappedSpeed);
      break;
    case State::Left:
      Rp6.moveAtSpeed(mappedSpeed * invertedRotateSpeed, mappedSpeed);
      break;
    case State::Right:
      Rp6.moveAtSpeed(mappedSpeed, mappedSpeed * invertedRotateSpeed);
      break;
    default:
      Rp6.moveAtSpeed(0, 0); // maybe Rp6.stop()?
      break;
  }
}

