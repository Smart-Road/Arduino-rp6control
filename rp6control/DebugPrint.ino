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
