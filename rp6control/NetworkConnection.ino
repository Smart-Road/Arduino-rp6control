void networkConnection() 
{
  static size_t clientCounter = 0;
  static int connectionTryCounter = 0;
  const int connectionTries = 1;

  connectionWithPc = pcClient.connected();
  if (!connectionWithPc && pcClientIp != IPAddress(0, 0, 0, 0) && connectionTryCounter < connectionTries) {

    connectionWithPc = true;
    if (pcClient.connect(pcClientIp, pcClientPortnumber)) {
      connectionTryCounter = 0;

      DEBUGCODE(Serial.println("hello connection with pc client!"));
      SendMessage(pcClient, "zone:15");
    } else {
      connectionTryCounter++;
      DEBUGCODE(Serial.println("trycounter got up by one"));
    }
  } else if (connectionTryCounter == 1) { // only for debugging
    connectionTryCounter++;
    DEBUGCODE(Serial.println("Stopped trying to connect"));
  }

  if (server.hasClient())
  {
    if (client[clientCounter] == 0 ||
        !client[clientCounter].connected())
    {
      client[clientCounter] = server.available();
      DEBUGCODE(Serial.printf("Client(%d) has connected.\n", clientCounter));
      connectionlist[clientCounter] = true;
    }
    else
    {
      DEBUGCODE(Serial.printf("ERROR: Client(%d) still connected.\nTrying next client.\n", clientCounter));
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
          DEBUGCODE(Serial.println("Controller disconnected."));
          controller = notInitializedController;
          connectionTryCounter = 0;
          // reset connection with pc?
          //pcClientIp = IPAddress(0, 0, 0, 0);
        }
        DEBUGCODE(Serial.printf("Client(%d) has disconnected.\n", j));
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
