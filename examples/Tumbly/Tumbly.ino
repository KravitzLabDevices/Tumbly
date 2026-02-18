/********************************************************
  Simple Castle with OLED Display - Tumbly Example
  Tumble feeder that controls the door based on RTC time.
  Data is logged to SD card.
  Modified for OLED display by Mason Barrett
  
  Hardware: Adafruit Feather M0, OLED FeatherWing (SH1107),
  RTC FeatherWing, SD card
 ********************************************************/

#include <Tumbly.h>

void setup() {
  // Optional: configure before begin()
  // setDeviceId(1);
  // setSleepTime(10);
  // setDoorTimes(20, 4);   // Open 8pm, close 4am
  // setDoorPositions(-10, 100);
  
  begin();
}

void loop() {
  ReadSensors();
  TimedDoor();
  LogData();
  UpdateDisplay();
  GoToSleep();
}
