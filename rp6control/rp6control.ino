#include <SerialCommunication.h>

/*
   Only works with one concurrent connection.
   Based on the Arduino reference WiFi examples,
   as seen here: http://www.arduino.cc/en/Reference/WiFiServerWrite
   It has been modified to work with the ESP8266.

   Made by: Rick van Schijndel
   Creation date: 8-4-2016
   Last modified: 1-6-2016
*/

//For the people that downloaded this from github, use the shareddata header.
//I've done it like this so my credentials are hidden for the scary internet.
//#include "privatedata.h"
#include "shareddata.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Rp6.h>

enum class State { Forward, Backward, Left, Right, Startup };
State state = State::Startup;
SoftwareSerial Comport(D7, D8);

int robotSpeed = 0;
float invertedRotateSpeed = 0; // can be 0 through 1, the higher it is, the slower the robot rotates
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
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  connection();
  robotAction();
}

void robotAction() {
  int leftSpeed = robotSpeed;
  int rightSpeed = robotSpeed;
  switch (state) {
    case State::Forward:
    case State::Backward:
      // keep it at robotSpeed
      break;
    case State::Left:
      leftSpeed *= invertedRotateSpeed;
      break;
    case State::Right:
      rightSpeed *= invertedRotateSpeed;
      break;
    default:
      rightSpeed *= 0;
      leftSpeed *= 0;
  }
  Rp6.moveAtSpeed(leftSpeed, rightSpeed);
}

void connection() {
  static size_t clientCounter = 0;
  if (server.hasClient())
  {
    if (client[clientCounter] == 0 ||
        !client[clientCounter].connected())
    {
      client[clientCounter] = server.available();
      Serial.printf("Client(%d) has connected.\n", clientCounter);
      connectionlist[clientCounter] = true;
    }
    else
    {
      Serial.printf("ERROR: Client(%d) still connected.\n", clientCounter);
      Serial.println("Trying next client.");
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
        Serial.printf("Client(%d) has disconnected.\n", j);
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
      Serial.println("NACK");
    }
  }
}

void useCommandWithParams(const String command) {
  int endCommand = command.indexOf(":");
  String beginCommand = command.substring(0, endCommand);
  String parameter = command.substring(endCommand + 1);
  Serial.println("begincommand:" + beginCommand + ", parameter:" + parameter);

  if (beginCommand == "SPEED") {
    setRobotSpeed(parameter.toInt());
  }
  
  if (beginCommand == "DIRECTION") {
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
      Serial.println("NACK");
    }
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
  Serial.printf("Speed set to %d\n", robotSpeed);
}

