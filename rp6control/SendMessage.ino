#define BEGINDELIMITER '%'
#define ENDDELIMITER '$'

void SendMessage(WiFiClient wfclient, String message) {
  String messageToSend = BEGINDELIMITER + message + ENDDELIMITER;
  wfclient.print(messageToSend);
  DEBUGCODE(Serial.println("Sent message " + messageToSend + " to client"));
}

#undef BEGINDELIMITER
#undef ENDDELIMITER
