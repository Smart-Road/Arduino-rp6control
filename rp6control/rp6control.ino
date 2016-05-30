#include <SerialCommunication.h>

/*
   Only works with one concurrent connection.
   Based on the Arduino reference WiFi examples,
   as seen here: http://www.arduino.cc/en/Reference/WiFiServerWrite
   It has been modified to work with the ESP8266.

   Made by: Rick van Schijndel
   Creation date: 8-4-2016
   Last modified: 10-4-2016
*/

//For the people that downloaded this from github, use the shareddata header.
//I've done it like this so my credentials are hidden for the scary internet.
#include "privatedata.h"
//#include "shareddata.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Rp6.h>

enum class State { Forward, Backward, Left, Right, Startup };
State state = State::Startup;
SoftwareSerial Comport(D7, D8);

int robotSpeed = 0;

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
    clientCommunication[i].begin(client[i], '#', '%');
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
      leftSpeed *= 0.5;
      break;
    case State::Right:
      rightSpeed *= 0.5;
      break;
  }
  if (millis() % 2000 == 0) {
    Serial.printf("LEFT: %d, RIGHT: %d\n", leftSpeed, rightSpeed);
  }
  //Rp6.moveAtSpeed(leftSpeed, rightSpeed);
}

void connection() {
  static size_t clientCounter = 0;
  if (server.hasClient())
  {
    if (client[clientCounter] == NULL || !client[clientCounter].connected())
    {
      client[clientCounter] = server.available();
      //Serial.printf("Client(%d) has connected.\n", clientCounter);
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
        Serial.println(commandline);
      }
    }
    else if (connectionlist[j])
    {
      connectionlist[j] = client[j].connected();
      if (!connectionlist[j])
      {
        //Serial.printf("Client(%d) has disconnected.\n", j);
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
    }
    else
    {
      if (command == "RIGHT")
      {
        state = State::Right;
      }
      else if (command == "LEFT")
      {
        state = State::Left;
      }
      else if (command == "BACKWARD")
      {
        // TODO: set robot direction to backwards
        state = State::Backward;
      }
      else if (command == "FORWARD") {
        // TODO: set robot direction to forward
        state = State::Forward;
      }
      else
      {
        Serial.println("NACK");
      }
    }
  }
}

void useCommandWithParams(const String command) {
  int endCommand = command.indexOf(":");
  String beginCommand = command.substring(0, endCommand);
  String parameter = command.substring(endCommand + 1);
  Serial.println(beginCommand + " " + parameter);

  if (beginCommand == "SPEED") {
    setRobotSpeed(parameter.toInt());
  }
}

void setRobotSpeed(int speedParam) {
  if (speedParam > 0 || speedParam < 200) {
    robotSpeed = speedParam;
  } else if (speedParam > 200) {
    robotSpeed = 200;
  } else {
    robotSpeed = 0;
  }
  Serial.printf("Speed set to %d\n", robotSpeed);
}

