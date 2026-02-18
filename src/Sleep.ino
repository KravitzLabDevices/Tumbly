/********************************************************
  Sleep and wakeup
********************************************************/
void GoToSleep() {
  Serial.print("Sleeping...");
  digitalWrite(LED_BUILTIN, LOW);

  display.oled_command(SH110X_DISPLAYOFF);

  LowPower.sleep(sleeptime * 1000);  // wake every X ms

  Serial.println("Awake!");
  display.oled_command(SH110X_DISPLAYON);

  digitalWrite(LED_BUILTIN, HIGH);
}
