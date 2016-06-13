/*
	Created by: Rick van Schijndel
	Version: 1.0.0
*/

#ifndef SerialCommunication_h
#define SerialCommunication_h
#include <Arduino.h>

#define BEGINDELIMITER '%'
#define ENDDELIMITER '$'
#define MAX_COMMAND_LENGTH 20

enum class Communication { WaitForStart, Receiving };


class SerialCommunication {
  private:
	bool _isStarted;
	char _beginDelimiter;
	char _endDelimiter;
    String _data;
    char _incomingChar;
    String _receivedString;
    Stream* _deviceToCommunicateWith;
    Communication _state;
    String _lastCommand;
    void reset();
  public:
	SerialCommunication();
	void begin(Stream &theStream, char beginDelimiter = '%', char endDelimiter = '$');
    String getCommand();
    bool update();
};

#endif
