# Step 4: Flash the code

## 4.1 Set up the Arduino IDE
1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Follow [Adafruit's instructions](https://learn.adafruit.com/adafruit-feather-m0-express-designed-for-circuit-python-circuitpython/arduino-ide-setup) to add support for the Feather M0 board.

## 4.2 Install the Tumbly library
1. In the Arduino IDE, click **Sketch → Include Library → Manage Libraries**.
2. Search for **Tumbly**.
3. Click **Install**. If it asks, also install the dependencies.

<img width="200" alt="Tumbly in the Library Manager" src="https://github.com/user-attachments/assets/b980073e-310c-41cb-a829-7d9c3e1a2b51" />

## 4.3 Upload the sketch
1. Open **File → Examples → Tumbly → Tumbly**.
2. Plug the Feather into your computer with a micro-USB **data** cable.
3. Double-click the reset button on the Feather to put it in bootloader mode.
4. Under **Tools → Board**, select **Adafruit Feather M0**.

   <img width="200" alt="Board selection" src="https://github.com/user-attachments/assets/a0a9602e-b9f5-4bbb-a4bf-8c812cc7041d" />

5. Click the **Upload** arrow.

   <img width="40" alt="Upload button" src="https://github.com/user-attachments/assets/c16998c5-c064-45a0-a1cc-0b1df94467e9" />

## The example sketch
You can leave the sketch as it is. Task, schedule, device ID, and door positions can all be changed on the device (see [Step 6](Use-Tumbly)).

```cpp
#include <Tumbly.h>

// Task options: "TimedDoor", "FreeFeeding", or "Demo"
String task = "TimedDoor";
bool darkMode = false;   // true = turn off screen and LED after the task starts
Tumbly tumbly(task, darkMode);

void setup() {
  tumbly.begin();
}

void loop() {
  tumbly.run();
}
```

---
[← Step 3: Assemble the electronics](Assemble-Electronics) | **Next →** [Step 5: Assemble Tumbly](Assemble-Tumbly)
