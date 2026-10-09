# Step 6: Set up and use Tumbly

## Buttons
Tumbly has three buttons on the OLED FeatherWing: **A**, **B**, and **C**. The labels at the bottom of the screen show what each button does on that screen.

## First-time setup
1. Put a microSD card in the Feather and turn Tumbly on. If no card is found, the screen says **NO SD CARD!**.
2. The settings screen shows the current device ID, task, open/close times, and door positions.
   - Press **A** to start with these settings.
   - Press **C** to edit them.
3. When you edit, Tumbly takes you through these screens in order:
   1. **Task:** A cycles through the options, C selects.
   2. **Device ID:** A increases it, B decreases it, C goes to the next screen.
   3. **Open time** and **Close time** (30-minute steps): A moves later, B moves earlier, C sets the time.
   4. **Open position** and **Closed position:** move the hopper to each position by hand, then press **C** to lock it. If a position is already saved, you can press **A** to keep it.
4. Settings are saved to `config.txt` on the SD card and loaded again the next time Tumbly starts.

> **Tip:** Run the **Demo** task first to confirm the door moves correctly before you start a real experiment.

## Tasks
| Task | What it does |
|---|---|
| **TimedDoor** | Opens the hopper at the open time and closes it at the close time. Default: open 20:00, close 04:00. |
| **FreeFeeding** | Keeps the door open all the time. |
| **Demo** | Hardware test. Wakes every 5 s and switches between open and closed so you can watch the servo work. |

In every task, Tumbly checks the servo position regularly and corrects it if it has moved. If it cannot correct the position, the screen shows **SERVO ERROR – Check for jam**. Clear the jam and press **B** to resume.

## Dark mode
Set `darkMode = true` in the sketch to turn off the screen and LED once the task starts. Press **A** at any time to wake the display for one cycle.

## Data
Each session writes a new CSV file to the SD card, named `TUMBLY<ID>_<MMDDYY>_<NN>.csv`, with these columns:

`Datetime, Device_Number, Task, Battery_Voltage, Light Sensor, DoorOpen, Servo_Feedback, Error`

---
[← Step 5: Assemble Tumbly](Assemble-Tumbly) | [Home](Home) | [Power consumption](Power-Consumption)
