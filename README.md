# Tumbly

An Arduino library for a tumble feeder optimized for time-restricted feeding. Controls a servo-operated door based on RTC time or light level, with OLED display and SD card logging.

**Hardware:** Adafruit Feather M0, OLED FeatherWing (SH1107), RTC FeatherWing, SD card, servo

Based on the [Tumble Feeder research](https://www.biorxiv.org/content/10.1101/2024.10.01.615599v1).

## Installation

### Arduino Library Manager (recommended)

1. Open Arduino IDE
2. **Sketch** → **Include Library** → **Manage Libraries**
3. Search for "Tumbly"
4. Click **Install**

## Usage
5. Click Examples > Tumbly > Tumbly.ino
6. Flash to your Tumbly!
      
```cpp
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
```
## License

GPL-3.0
