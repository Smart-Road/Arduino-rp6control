void connectWifi() {
  DEBUGCODE(Serial.println());
  DEBUGCODE(Serial.println());
  DEBUGCODE(Serial.printf("Connecting to %s\n", ssid));

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DEBUGCODE(Serial.print("."));
  }
  DEBUGCODE(Serial.println());
  DEBUGCODE(Serial.println("WiFi connected"));

  // Print the IP address
  Serial.println(WiFi.localIP()); // print this always
}

void startServer() {
  server.begin();
  DEBUGCODE(Serial.println("Server started"));
}

