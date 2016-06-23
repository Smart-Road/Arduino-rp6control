void robotAction() 
{
  switch (state) {
    case State::Forward:
    case State::Backward:
      // keep it at robotSpeed
      Rp6.moveAtSpeed(robotSpeed, robotSpeed);
      break;
    case State::Left:
      Rp6.moveAtSpeed(robotSpeed * invertedRotateSpeed, robotSpeed);
      break;
    case State::Right:
      Rp6.moveAtSpeed(robotSpeed, robotSpeed * invertedRotateSpeed);
      break;
    default:
      Rp6.moveAtSpeed(0, 0); // maybe Rp6.stop()?
      break;
  }
}
