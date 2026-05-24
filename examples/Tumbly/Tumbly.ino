#include <Tumbly.h>

// Set task to one of the following (can also be changed in the settings menu on device):
//
//   "TimedDoor"   — opens the hopper at openHour and closes it at closeHour.
//                   Servo position is verified every 5 minutes and corrected if needed.
//
//   "FreeFeeding" — door stays open at all times.
//                   Servo position is still verified every 5 minutes.
//
//   "Demo"        — fast validation loop for hardware testing. Wakes every 5 seconds,
//                   checks position every 10 seconds, alternates open/closed so you
//                   can confirm servo movement before starting a real task.
//
// Set darkMode to true to suppress all screen and LED output after the task starts.
// Press button A at any time to wake the display for one cycle.
// Set darkMode to false to leave screen and LED behavior unchanged.

String task = "TimedDoor";
bool darkMode = false;
Tumbly tumbly(task, darkMode);

void setup() {
  tumbly.begin();
}

void loop() {
  tumbly.run();
}
