void rfidCheck() 
{
  if (rfidReader.update()) {
    long commands[2];
    String sCommand = rfidReader.getCommand();
    if (!parseCommand(sCommand, commands)) {
      DEBUGCODE(Serial.println("Invalid command received:" + sCommand));
      return;
    }
    long command = commands[0];
    
    if (command != RFID) {
      DEBUGCODE(Serial.printf("Other command than expected received:%ld", command));
      return;
    }

    String rfid = getParamString(sCommand);
    
    DEBUGCODE(Serial.println("Rfid read:" + rfid));
    if (pcClient.connected()) {
      SendMessage(pcClient, "GETSPEED:" + rfid);
    }
  }
}

