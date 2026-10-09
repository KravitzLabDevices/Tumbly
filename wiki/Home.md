Tumbly is a low-cost, battery-powered **time-restricted feeding device** for home-cage use. A servo rotates the food hopper open or closed on a schedule, and every event is logged to an SD card. It is based on the [Tumble Feeder](https://pubmed.ncbi.nlm.nih.gov/40541188/).

## 🎬 Build video
Watch the full build start to finish, then use the steps below as a reference while you work.

[![Tumbly build video](https://img.youtube.com/vi/ViJIuEpGvDw/hqdefault.jpg)](https://youtu.be/ViJIuEpGvDw)

## Build steps
Follow these in order. The sidebar has the same links on every page.

| Step | What you'll do |
|---|---|
| **1. [Gather parts](Parts-List)** | Order the components, PCB, and battery |
| **2. [3D print the parts](3D-prints)** | Print the base, hopper, and door |
| **3. [Assemble the electronics](Assemble-Electronics)** | Populate the custom PCB and stack the Feather boards |
| **4. [Flash the code](Flash-code)** | Install the Tumbly Arduino library and upload the sketch |
| **5. [Assemble Tumbly](Assemble-Tumbly)** | Mount the electronics and servo in the printed parts |
| **6. [Set up and use Tumbly](Use-Tumbly)** | Calibrate the door, set the schedule, and read your data |

## Hardware at a glance
- Adafruit Feather M0 Adalogger (microcontroller + SD card)
- Adafruit DS3231 RTC FeatherWing (real-time clock)
- Adafruit 128x64 OLED FeatherWing (SH1107)
- Custom Tumbly PCB
- Feedback servo
- LiPo battery (a 2200 mAh battery runs ~50–100 days; see [Power consumption](Power-Consumption))

---
**Next →** [Step 1: Gather parts](Parts-List)
