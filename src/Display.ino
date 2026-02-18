/********************************************************
  Update OLED Display
********************************************************/
void UpdateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  
  // Line 1: Device name
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("CAS");
  display.println(CAS);
  
  // Line 2: Time & Date
  display.setTextSize(1);
  DateTime now = rtc.now();
  display.setCursor(0, 18);
  if (now.month() < 10) display.print('0');
  display.print(now.month());
  display.print('/');
  if (now.day() < 10) display.print('0');
  display.print(now.day());
  display.print('/');
  display.print(now.year());
  display.print("  ");
  if (now.hour() < 10) display.print('0');
  display.print(now.hour());
  display.print(':');
  if (now.minute() < 10) display.print('0');
  display.print(now.minute());
  
  // Line 3: Light level
  display.setCursor(0, 30);
  display.print("Light: ");
  display.print(lux);
  
  // Line 4: Battery voltage
  display.setCursor(0, 42);
  display.print("Battery: ");
  display.print(measuredvbat, 2);
  display.print("V");
  
  // Line 5: Door status (large)
  display.setCursor(0, 54);
  display.print("Door: ");
  if (doorOpen) {
    display.print("OPEN");
  } else {
    display.print("CLOSED");
  }
  
  display.display();
  delay (1000);
}