#include "SerialCommunication.h"

/*
   Works with 20 concurrent connections.
   Based on the Arduino reference WiFi examples,
   as seen here: http://www.arduino.cc/en/Reference/WiFiServerWrite
   It has been modified to work with the ESP8266.

   Made by: Rick van Schijndel
   Creation date: 8-4-2016
   Last modified: 23-6-2016
   by: Rick van Schijndel
*/

// define all commands
#define SPEED (1)
#define ANGLE (2)
#define DIRECTION (3)
#define CONTROL (4)
#define SERVERIP (5)
#define RFID (6)
#define MAXSPEED (7)

#define RIGHT (10)
#define LEFT (11)
#define FORWARD (12)
#define BACKWARD (13)

//For the people that downloaded this from github, use the shareddata header.
//I've done it like this so my credentials are hidden for the scary internet.
//#include "privatedata.h"
#include "shareddata.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Rp6.h>

#define DEBUG (1)

#if DEBUG == 1
#define DEBUGCODE(x) (x) // add ; to the end, because it did not look good if ; was between the () of the macro
#else
#define DEBUGCODE(x) // nothing
#endif

#define MAXSPEEDMIN (5)
#define MAXSPEEDMAX (130)

enum class State { Forward, Backward, Left, Right, Startup };
State state = State::Startup;
SoftwareSerial SerialRfidReader(D7, D8);

int cruiseSpeed = 0;
float invertedRotateSpeed = 1.0; // can be 0 to 1, the higher it is, the slower the robot rotates

int maxSpeed = 130; // init max speed on 130

const int serverPortnumber = 80;
WiFiServer server(serverPortnumber);

const uint32_t maxClientCount = 10;
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
SerialCommunication rfidReader;

void setup() 
{
  // setup all the SerialCommunication objects
  for (uint32_t i = 0; i < maxClientCount; i++)
  {
    clientCommunication[i].begin(client[i], '%', '$');
  }
  
  // start the Rp6 / init library
  Rp6.begin();
  
  // init array on false
  for (uint32_t i = 0; i < clientsize; i++) {
    connectionlist[i] = false;
  }
  
  Serial.begin(115200);
  SerialRfidReader.begin(9600);

  // initialize rfid reader sercom object
  rfidReader.begin(SerialRfidReader, '%', '$');

  connectWifi();
  startServer();
}

void loop() 
{
  networkConnection();
  robotAction();
  rfidCheck();
}

