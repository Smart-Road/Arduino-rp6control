#include "SerialCommunication.h"

/*
   Only works with one concurrent connection.
   Based on the Arduino reference WiFi examples,
   as seen here: http://www.arduino.cc/en/Reference/WiFiServerWrite
   It has been modified to work with the ESP8266.

   Made by: Rick van Schijndel
   Creation date: 8-4-2016
   Last modified: 13-6-2016
*/

//For the people that downloaded this from github, use the shareddata header.
//I've done it like this so my credentials are hidden for the scary internet.
//#include "privatedata.h"
#include "shareddata.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Rp6.h>

#define DEBUG 1

enum class State { Forward, Backward, Left, Right, Startup };
State state = State::Startup;
SoftwareSerial Comport(D7, D8);

int robotSpeed = 0;
float invertedRotateSpeed = 0; // can be 0 to 1, the higher it is, the slower the robot rotates

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

const uint32_t maxClientCount = 20;

WiFiClient client[maxClientCount];
const size_t clientsize = sizeof(client) / sizeof (client[0]);
bool connectionlist[clientsize];

SerialCommunication clientCommunication[maxClientCount];

void setup() {
  // setup all the SerialCommunication objects
  for (uint32_t i = 0; i < maxClientCount; i++)
  {
    clientCommunication[i].begin(client[i], '%', '$');
  }
  // start of the Rp6
  Rp6.begin();
  // init array on false
  for (uint32_t i = 0; i < clientsize; i++)
  {
    connectionlist[i] = false;
  }
  Serial.begin(9600);

  Comport.begin(9600);
  // Connect to WiFi network
  DebugPrintln("");
  DebugPrintln("");
  DebugPrint("Connecting to ");
  DebugPrintln(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DebugPrint(".");
  }
  DebugPrintln("");
  DebugPrintln("WiFi connected");

  // Start the server
  server.begin();
  DebugPrintln("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP()); // does not work with debugprintln, and is pretty handy anyway
}

void loop() {
  connection();
  robotAction();
}

void robotAction() {
  switch (state) {
    case State::Forward:
    case State::Backward:
      // keep it at robotSpeed
      Rp6.moveAtSpeed(robotSpeed, robotSpeed);
      break;
    case State::Left:
      //Rp6.rotate(robotSpeed, RP6_LEFT, 10); // 10 degrees, will this work right?
      Rp6.moveAtSpeed(robotSpeed * invertedRotateSpeed, robotSpeed);
      break;
    case State::Right:
      //Rp6.rotate(robotSpeed, RP6_RIGHT, 10); // same as above
      Rp6.moveAtSpeed(robotSpeed, robotSpeed * invertedRotateSpeed);
      break;
    default:
      Rp6.moveAtSpeed(0, 0);
      break;
  }
}

void connection() {
  static size_t clientCounter = 0;
  if (server.hasClient())
  {
    if (client[clientCounter] == 0 ||
        !client[clientCounter].connected())
    {
      client[clientCounter] = server.available();
      //Serial.printf("Client(%d) has connected.\n", clientCounter);
      DebugPrint("Client("); 
      DebugPrint(String(clientCounter));
      DebugPrintln(") has connected.");
      connectionlist[clientCounter] = true;
    }
    else
    {
      //Serial.printf("ERROR: Client(%d) still connected.\n", clientCounter);
      DebugPrint("ERROR: Client(");
      DebugPrint(String(clientCounter));
      DebugPrintln(") still connected.");
      DebugPrintln("Trying next client.");
    }
    clientCounter++;
    clientCounter = clientCounter % clientsize;
  }
  for (uint32_t j = 0; j < clientsize; j++)
  {
    String commandline;
    if (client[j].available() > 0)
    {
      if (clientCommunication[j].update())
      {
        commandline = clientCommunication[j].getCommand();
      }
    }
    else if (connectionlist[j])
    {
      connectionlist[j] = client[j].connected();
      if (!connectionlist[j])
      {
        //Serial.printf("Client(%d) has disconnected.\n", j);
        DebugPrint("Client(");
        DebugPrint(String(j));
        DebugPrint(") has disconnected.");
      }
    }
    useCommand(commandline);
    commandline = "";
  }
}

void useCommand(const String command) {
  if (command != NULL) {
    int paramcheck = command.indexOf(":");
    if (paramcheck > 0) {
      useCommandWithParams(command);
    } else {
      DebugPrintln("NACK");
    }
  }
}

void useCommandWithParams(const String command) {
  int endCommand = command.indexOf(":");
  String beginCommand = command.substring(0, endCommand);
  String parameter = command.substring(endCommand + 1);
  DebugPrintln("begincommand:" + beginCommand + ", parameter:" + parameter);

  if (beginCommand == "SPEED") {
    setRobotSpeed(parameter.toInt());
  }
  else if (beginCommand == "ANGLE") {
    setTurningAngle(parameter.toInt());
  }
  else if (beginCommand == "DIRECTION") {
    if (parameter == "RIGHT")
    {
      state = State::Right;
    }
    else if (parameter == "LEFT")
    {
      state = State::Left;
    }
    else if (parameter == "BACKWARD")
    {
      Rp6.changeDirection(RP6_BACKWARD);
      state = State::Backward;
    }
    else if (parameter == "FORWARD") {
      Rp6.changeDirection(RP6_FORWARD);
      state = State::Forward;
    }
    else
    {
      DebugPrintln("NACK");
    }
  }
  else {
    DebugPrintln("NACK");
  }
}

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

void DebugPrint(String message) {
#if DEBUG
  Serial.print(message);
#endif
}

void DebugPrintln(String message) {
#if DEBUG
  Serial.println(message);
#endif
}

