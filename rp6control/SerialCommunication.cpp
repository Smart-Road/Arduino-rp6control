/*
   class handling the serial communication
   when anything is available, it will check if it is a begindelimiter.
   When it is a begindelimiter, it will clear the receivedstring.
   If it is anything else, it will read it into a buffer, called receivedString

*/
#include "SerialCommunication.h"


SerialCommunication::SerialCommunication() {
  _isStarted = false;
  _receivedString = "";
  _incomingChar = '\0';
  _data = "";
  _state = Communication::WaitForStart;
  _lastCommand = "";
}

String SerialCommunication::getCommand() {
  if (!_isStarted) { return "NOT_STARTED_ERROR"; }
  return _lastCommand;
}

void SerialCommunication::begin(Stream &theStream, char beginDelimiter, char endDelimiter) {
  _deviceToCommunicateWith = &theStream;
  _beginDelimiter = beginDelimiter;
  _endDelimiter = endDelimiter;
  _isStarted = true;
}

bool SerialCommunication::update() {
  if (!_isStarted) {
	return false;
  }
  while (_deviceToCommunicateWith->available() > 0) {
    _incomingChar = _deviceToCommunicateWith->read();
    switch (_state) {
      case Communication::WaitForStart:
        if (_incomingChar == _beginDelimiter) {
          _receivedString = "";
          _state = Communication::Receiving;
        }
        break;
      case Communication::Receiving:
        if (_incomingChar == _endDelimiter) {
          _lastCommand = _receivedString;
          reset();
          return true;
        } else {
          _receivedString += _incomingChar;
        }

        if (_receivedString.length() > MAX_COMMAND_LENGTH) {
          _lastCommand = "LENGTH_ERROR";
          reset();
          return true;
        }
        break;
    }
  }
  return false;
}

void SerialCommunication::reset() {
  _receivedString = "";
  _state = Communication::WaitForStart;
}

