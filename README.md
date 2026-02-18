# Tumbly

An Arduino library for a tumble feeder optimized for time-restricted feeding. 

**Hardware:** Adafruit Feather M0, OLED FeatherWing (SH1107), RTC FeatherWing, SD card, Servo  

See the [Wiki](https://github.com/KravitzLabDevices/Tumbly/wiki) for build instructions.

Based on the [Tumble Feeder](https://pubmed.ncbi.nlm.nih.gov/40541188/).

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
