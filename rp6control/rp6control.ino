#include "SerialCommunication.h"

/*
   Works with 20 concurrent connections.
   Based on the Arduino reference WiFi examples,
   as seen here: http://www.arduino.cc/en/Reference/WiFiServerWrite
   It has been modified to work with the ESP8266.

   Made by: Rick van Schijndel
   Creation date: 8-4-2016
   Last modified: 22-6-2016
   by: Rick van Schijndel
*/

//For the people that downloaded this from github, use the shareddata header.
//I've done it like this so my credentials are hidden for the scary internet.
//#include "privatedata.h"
#include "shareddata.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Rp6.h>

#define DEBUG (1)

#if DEBUG == 1
#define DEBUG(x) (x);
#else
#define DEBUG(x) ;// nothing
#endif

enum class State { Forward, Backward, Left, Right, Startup };
State state = State::Startup;
SoftwareSerial Comport(D7, D8);

int robotSpeed = 0;
float invertedRotateSpeed = 1.0; // can be 0 to 1, the higher it is, the slower the robot rotates

const int serverPortnumber = 80;
WiFiServer server(serverPortnumber);

const uint32_t maxClientCount = 20;
WiFiClient client[maxClientCount];
const size_t clientsize = sizeof(client) / sizeof (client[0]);

bool connectionlist[clientsize];

const uint32_t notInitializedController = 255;
uint32_t controller = notInitializedController;

const int pcClientPortnumber = 13;
IPAddress pcClientIp(0, 0, 0, 0); // should be 0,0,0,0
WiFiClient pcClient;
bool connectionWithPc = false;

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
  Serial.begin(115200);

  Comport.begin(9600);
  // Connect to WiFi network
  DEBUG(Serial.println())
  DEBUG(Serial.println())
  DEBUG(Serial.printf("Connecting to %s\n", ssid))

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DEBUG(Serial.print("."))
  }
  DEBUG(Serial.println())
  DEBUG(Serial.println("WiFi connected"))

  // Start the server
  server.begin();
  DEBUG(Serial.println("Server started"))

  // Print the IP address
  Serial.println(WiFi.localIP()); // print this always
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

void connection() {
  static size_t clientCounter = 0;
  static int connectionTryCounter = 0;
  const int connectionTries = 1;

  connectionWithPc = pcClient.connected();
  if (!connectionWithPc && pcClientIp != IPAddress(0, 0, 0, 0) && connectionTryCounter < connectionTries) {

    connectionWithPc = true;
    if (pcClient.connect(pcClientIp, pcClientPortnumber)) {
      connectionTryCounter = 0;

      DEBUG(Serial.println("hello connection with pc client!"))
      SendMessage(pcClient, "zone:15");
    } else {
      connectionTryCounter++;
      DEBUG(Serial.println("trycounter got up by one"))
    }
  } else if (connectionTryCounter == 1) { // only for debugging
    connectionTryCounter++;
    DEBUG(Serial.println("Stopped trying to connect"))
  }

  if (server.hasClient())
  {
    if (client[clientCounter] == 0 ||
        !client[clientCounter].connected())
    {
      client[clientCounter] = server.available();
      DEBUG(Serial.printf("Client(%d) has connected.\n", clientCounter))
      connectionlist[clientCounter] = true;
    }
    else
    {
      DEBUG(Serial.printf("ERROR: Client(%d) still connected.\nTrying next client.\n", clientCounter))
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
        if (j == controller) {
          DEBUG(Serial.println("Controller disconnected."))
          controller = -1;
          connectionTryCounter = 0;
          // reset connection with pc?
          //pcClientIp = IPAddress(0, 0, 0, 0);
        }
        DEBUG(Serial.printf("Client(%d) has disconnected.\n", j))
      }
    }
    if (commandline.length() > 0) {
      if (j == controller) {
        useCommand(commandline);
      } else if ( controller == notInitializedController) { // if controller is not set and a message is received
        if (checkAndSetController(commandline, j)) {
          SendMessage(client[j], "CONTROL:GRANTED");
        } else { // message was wrong (not CONTROL:)
          SendMessage(client[j], "ACCESS:DENIED");
          client[j].stop();
        }
      } else { // if client is not the controller and the controller is already set
        SendMessage(client[j], "ACCESS:DENIED");
        client[j].stop();
      }
      commandline = "";
    }
  }
}

