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
//#include "privatedata.h"
#include "shareddata.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Rp6.h>

SoftwareSerial Comport(D7, D8);

String commandline;



// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

const int maxClientCount = 20;

WiFiClient client[maxClientCount];
const size_t clientsize = sizeof(client) / sizeof (client[0]);
bool connectionlist[clientsize];

  SerialCommunication clientCommunication[] {
  SerialCommunication(&client[0], '#', '%'),
  SerialCommunication(&client[1], '#', '%'),
  SerialCommunication(&client[2], '#', '%'),
  SerialCommunication(&client[3], '#', '%'),
  SerialCommunication(&client[4], '#', '%'),
  SerialCommunication(&client[5], '#', '%'),
  SerialCommunication(&client[6], '#', '%'),
  SerialCommunication(&client[7], '#', '%'),
  SerialCommunication(&client[8], '#', '%'),
  SerialCommunication(&client[9], '#', '%'),
  SerialCommunication(&client[10], '#', '%'),
  SerialCommunication(&client[11], '#', '%'),
  SerialCommunication(&client[12], '#', '%'),
  SerialCommunication(&client[13], '#', '%'),
  SerialCommunication(&client[14], '#', '%'),
  SerialCommunication(&client[15], '#', '%'),
  SerialCommunication(&client[16], '#', '%'),
  SerialCommunication(&client[17], '#', '%'),
  SerialCommunication(&client[18], '#', '%'),
  SerialCommunication(&client[19], '#', '%')
  };

void setup() {
  
  //start of the Rp6
  Rp6.begin();
  //init array on false
  for (int i = 0; i < clientsize; i++) {
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

  while (WiFi.status() != WL_CONNECTED) {
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
  static size_t clientCounter = 0;
  if (server.hasClient()) {
    if (client[clientCounter] == NULL || !client[clientCounter].connected()) {
      client[clientCounter] = server.available();
      Serial.printf("Client(%d) has connected.\n", clientCounter);
      connectionlist[clientCounter] = true;
    } else {
      Serial.printf("ERROR: Client(%d) still connected.\n", clientCounter);
      Serial.println("Trying next client.");
    }
    clientCounter++;
    clientCounter = clientCounter % clientsize;
  }
  for (size_t j = 0; j < clientsize; j++) {
    if (client[j].available() > 0) {
      if (clientCommunication[j].update()) {
        commandline = clientCommunication[j].getCommand();
        Serial.println(commandline);
      }
    } else if (connectionlist[j]) {
      connectionlist[j] = client[j].connected();
      if (!connectionlist[j]) {
        Serial.printf("Client(%d) has disconnected.\n", j);
      }
    }
  }

  if (commandline != NULL) {
    if (commandline == "RIGHT") {

      Serial.println("Right");
      Rp6.moveAtSpeed(100, 50);
      Serial.println("done");
      commandline = "";
    } else if (commandline == "LEFT") {

      Serial.println("Left");
      Rp6.moveAtSpeed(50, 100);
      Serial.println("done");
      commandline = "";
    } else {

      Serial.println("NACK");
      commandline = "";
      
    }
  }
}


