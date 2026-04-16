#include <Tumbly.h>
String task = "TimedDoor";
Tumbly tumbly(task);

void setup() {
  tumbly.begin();
}

void loop() {
  tumbly.run();
}
