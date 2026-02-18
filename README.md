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

### Manual install

1. Download or clone this repository
2. Copy the `Tumbly` folder to your Arduino `libraries` folder
3. Restart Arduino IDE

## Usage

```cpp
#include <Tumbly.h>

void setup() {
  // Optional: configure before begin()
  setDeviceId(1);           // Unique device ID (default: 1)
  setSleepTime(10);         // Seconds between readings (default: 10)
  setDoorTimes(20, 4);      // Open at 8pm, close at 4am
  setDoorPositions(-10, 100);
  
  begin();
}

void loop() {
  run();
}
```

## Getting Listed in Library Manager

To have your library appear in Arduino Library Manager:

1. Create a **GitHub release** with a version tag (e.g., `v1.0.0`)
2. Submit a PR to [arduino/library-registry](https://github.com/arduino/library-registry) adding your library's repo URL and the latest release
3. Follow the [Library Manager FAQ](https://github.com/arduino/Arduino/wiki/Library-Manager-FAQ) for details

## License

GPL-3.0
