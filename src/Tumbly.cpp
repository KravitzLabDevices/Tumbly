/********************************************************
  Tumbly - Tumble Feeder Library
  Copyright (c) 2024 Lex Kravitz, Mason Barrett
  Released under GPL-3.0
 ********************************************************/

#include "Tumbly.h"

static Tumbly* _instance = nullptr;
static bool _wokenByButton = false;
static void wakeupCallback() { _wokenByButton = true; }

static void _dateTimeCallback(uint16_t* date, uint16_t* time) {
  DateTime now = _instance->rtc.now();
  *date = FAT_DATE(now.year(), now.month(), now.day());
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

Tumbly::Tumbly(String& task) : display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire), task(task) {}

void Tumbly::begin() {
  _instance = this;

  Serial.begin(115200);
  Serial.println("Starting up... Welcome to Tumbly");

  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);

  if (!display.begin(0x3C, true)) {
    Serial.println(F("SH1107 allocation failed"));
  }
  display.setRotation(1);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Tumbly");
  display.setCursor(0, 10);
  display.print("Device: ");
  display.println(deviceId);
  display.setCursor(0, 20);
  display.println("Initializing...");
  display.display();
  delay(2000);

  myservo.attach(10);
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  rtc.begin();

  while (!SD.begin(_chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("No SD card found");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("NO SD CARD!");
    display.setCursor(0, 12);
    display.println("Insert SD card");
    display.println("to start.");
    display.display();
    delay(2000);
  }
  Serial.println("SD card initialized.");

  if (loadConfig()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Config loaded!");
    display.display();
    delay(1500);
  }

  int n = 0;
  DateTime now = rtc.now();
  snprintf(filename, sizeof(filename), "FEED%03d_%02d%02d%02d_%02d.csv", deviceId, now.month(), now.day(), now.year() - 2000, n);
  while (SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "FEED%03d_%02d%02d%02d_%02d.csv", deviceId, now.month(), now.day(), now.year() - 2000, n);
  }

  SdFile::dateTimeCallback(_dateTimeCallback);
  Serial.print("New file created: ");
  Serial.println(filename);
  writeHeader();
  Serial.println("Logfile initialized");

  SettingsMenu();

  open_door();
  delay(1000);
  close_door();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Ready!");
  display.setCursor(0, 10);
  display.print("Open: ");
  display.print(openHour);
  display.print("-");
  display.println(closeHour);
  display.setCursor(0, 20);
  display.print("File: ");
  display.println(filename);
  display.setCursor(0, 40);
  display.println("Press A to wake");
  display.setCursor(0, 50);
  display.println("device from sleep");
  display.display();
  delay(3000);
}

void Tumbly::run() {
  ReadSensors();
  if (taskIndex == 0) {
    TimedDoor();
  } else if (taskIndex == 1) {
    if (!doorOpen) open_door();
  }
  LogData();
  UpdateDisplay();
  GoToSleep();
}

void Tumbly::ReadSensors() {
  lux = analogRead(A3);
  Serial.print("Light: ");
  Serial.println(lux);
  analogReadResolution(10);
  measuredvbat = analogRead(A7);
  measuredvbat *= 2;
  measuredvbat *= 3.3;
  measuredvbat /= 1024;
}

void Tumbly::LightControlledDoor() {
  if (lux > 5 && doorOpen == false) open_door();
  else if (lux < 6 && doorOpen == true) close_door();
}

void Tumbly::TimedDoor() {
  rtc.begin();
  DateTime now = rtc.now();
  Serial.print("Hour: ");
  Serial.println(now.hour());

  if (now.year() > 2020) {
    bool inOpenWindow;
    if (openHour > closeHour) {
      inOpenWindow = (now.hour() >= openHour || now.hour() < closeHour);
    } else {
      inOpenWindow = (now.hour() >= openHour && now.hour() < closeHour);
    }
    if (inOpenWindow) {
      if (doorOpen == false) open_door();
    } else if (doorOpen == true) {
      close_door();
    }
  }
}

void Tumbly::open_door() {
  digitalWrite(11, HIGH);
  myservo.attach(10);
  for (int pos = closedpos; pos >= openpos; pos--) {
    myservo.write(pos);
    delay(30);
  }
  myservo.detach();
  digitalWrite(11, LOW);
  doorOpen = true;
}

void Tumbly::close_door() {
  digitalWrite(11, HIGH);
  myservo.attach(10);
  for (int pos = openpos; pos <= closedpos; pos++) {
    myservo.write(pos);
    delay(30);
  }
  myservo.detach();
  digitalWrite(11, LOW);
  doorOpen = false;
}

void Tumbly::UpdateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("Dev ");
  display.println(deviceId);
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
  display.setCursor(0, 30);
  display.print(lux > 50 ? "Dark" : "Light");
  display.setCursor(0, 42);
  display.print("Battery: ");
  display.print(measuredvbat, 2);
  display.print("V");
  display.setCursor(0, 54);
  if (!sdPresent) {
    display.print("!! NO SD CARD !!");
  } else {
    display.print("Door: ");
    display.print(doorOpen ? "OPEN" : "CLOSED");
  }
  display.display();
  delay(1000);
}

void Tumbly::writeHeader() {
  logfile = SD.open(filename, FILE_WRITE);
  logfile.println("Datetime,Device_Number,Battery_Voltage,Light Sensor,DoorOpen");
  logfile.close();
}

void Tumbly::saveConfig() {
  SD.remove("config.txt");
  File cfg = SD.open("config.txt", FILE_WRITE);
  if (!cfg) return;
  cfg.println(deviceId);
  cfg.println(taskIndex);
  cfg.println(openHour);
  cfg.println(closeHour);
  cfg.println(openpos);
  cfg.println(closedpos);
  cfg.println(sleeptime);
  cfg.close();
}

bool Tumbly::loadConfig() {
  File cfg = SD.open("config.txt", FILE_READ);
  if (!cfg) return false;
  deviceId  = cfg.parseInt();
  taskIndex = cfg.parseInt();
  openHour  = cfg.parseInt();
  closeHour = cfg.parseInt();
  openpos   = cfg.parseInt();
  closedpos = cfg.parseInt();
  sleeptime = cfg.parseInt();
  cfg.close();
  const char* tasks[] = {"TimedDoor", "FreeFeeding"};
  if (taskIndex >= 0 && taskIndex < 2) task = tasks[taskIndex];
  return true;
}

void Tumbly::LogData() {
  if (!SD.begin(_chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    sdPresent = false;
    return;
  }
  sdPresent = true;
  digitalWrite(8, HIGH);
  DateTime now = rtc.now();
  logfile = SD.open(filename, FILE_WRITE);
  logfile.print(now.month());
  logfile.print('/');
  logfile.print(now.day());
  logfile.print('/');
  logfile.print(now.year());
  logfile.print(" ");
  logfile.print(now.hour());
  logfile.print(':');
  logfile.print(now.minute());
  logfile.print(':');
  logfile.print(now.second());
  logfile.print(",");
  logfile.print(deviceId);
  logfile.print(",");
  logfile.print(measuredvbat);
  logfile.print(",");
  logfile.print(lux > 50 ? "Dark" : "Light");
  logfile.print(",");
  logfile.println(doorOpen);
  if (!logfile) error();
  logfile.close();
  digitalWrite(8, LOW);
}

void Tumbly::error() {
  digitalWrite(13, HIGH);
  digitalWrite(8, HIGH);
}

void Tumbly::GoToSleep() {
  Serial.print("Sleeping...");
  digitalWrite(LED_BUILTIN, LOW);
  display.oled_command(SH110X_DISPLAYOFF);
  _wokenByButton = false;
  LowPower.attachInterruptWakeup(RED_BUTTON, wakeupCallback, FALLING);
  LowPower.sleep(sleeptime * 1000);
  Wire.begin();
  Serial.println("Awake!");
  display.oled_command(SH110X_DISPLAYON);
  digitalWrite(LED_BUILTIN, HIGH);
  if (_wokenByButton) delay(10000);
}

void Tumbly::readButtons() {
  _redTouch   = (digitalRead(RED_BUTTON) == LOW);
  _greenTouch = (digitalRead(GREEN_BUTTON) == LOW);
  _blueTouch  = (digitalRead(BLUE_BUTTON) == LOW);
}

void Tumbly::beep() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
}

void Tumbly::SettingsMenu() {
  bool needsRedraw = true;
  while (true) {
    if (needsRedraw) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.print("Device: ");
      display.print(deviceId);
      display.setCursor(0, 8);
      display.print("Task: ");
      display.print(task);
      display.setCursor(0, 18);
      display.print("Open:  ");
      if (openHour < 10) display.print('0');
      display.print(openHour);
      display.print(":00");
      display.setCursor(0, 27);
      display.print("Close: ");
      if (closeHour < 10) display.print('0');
      display.print(closeHour);
      display.print(":00");
      display.setCursor(0, 36);
      display.print("O.Pos: ");
      display.print(openpos);
      display.setCursor(0, 45);
      display.print("C.Pos: ");
      display.print(closedpos);
      display.setCursor(0, 56);
      display.print("A:Start");
      display.setCursor(74, 56);
      display.print("C:Edit");
      display.display();
      needsRedraw = false;
    }
    readButtons();
    if (_redTouch) { beep(); saveConfig(); return; }
    if (_blueTouch) { beep(); delay(200); EditTask(); needsRedraw = true; }
    delay(50);
  }
}

void Tumbly::EditTask() {
  const char* tasks[] = {"TimedDoor", "FreeFeeding"};
  const int numTasks = 2;
  _endstate = false;
  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Select Task");
    display.setCursor(10, 22);
    display.print("TimedDoor");
    if (taskIndex == 0) display.print("  <");
    display.setCursor(10, 34);
    display.print("FreeFeeding");
    if (taskIndex == 1) display.print("  <");
    display.setCursor(0, 56);
    display.print("A:Change   C:Select");
    display.display();

    if (_redTouch) { beep(); taskIndex = (taskIndex + 1) % numTasks; delay(200); }
    if (_blueTouch) {
      beep();
      task = tasks[taskIndex];
      delay(200);
      _endstate = true;
      EditDeviceId();
    }
    delay(50);
  }
}

void Tumbly::EditDeviceId() {
  _endstate = false;
  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Set Device ID");
    display.setCursor(0, 20);
    display.setTextSize(3);
    if (deviceId < 10) display.print("00");
    else if (deviceId < 100) display.print("0");
    display.print(deviceId);
    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("A:+  B:-   C:Next");
    display.display();

    if (_redTouch) { beep(); deviceId++; if (deviceId > 20) deviceId = 1; delay(200); }
    if (_greenTouch) { beep(); deviceId--; if (deviceId < 1) deviceId = 999; delay(200); }
    if (_blueTouch) {
      beep(); delay(200); _endstate = true;
      if (taskIndex == 0) EditOpenHour();
      else EditOpenPosition();
    }
    delay(50);
  }
}

void Tumbly::EditOpenHour() {
  _endstate = false;
  while (!_endstate) {
    readButtons();
    display.clearDisplay();

    display.setCursor(0, 0);
    display.println("Set Open Time");
    display.setCursor(0, 10);
    display.setTextSize(2);
    if (openHour < 10) display.print('0');
    display.print(openHour);
    display.print(":00");
    display.setTextSize(1);

    int ox = (openHour * 128) / 24;
    display.setCursor(constrain(ox - 3, 0, 121), 28);
    display.print("v");

    display.drawRect(0, 36, 128, 6, SH110X_WHITE);

    for (int i = 0; i <= 4; i++) {
      int tx = constrain(i * 32, 0, 127);
      display.drawFastVLine(tx, 42, 3, SH110X_WHITE);
    }
    display.setCursor(0, 46);   display.print("0");
    display.setCursor(29, 46);  display.print("6");
    display.setCursor(58, 46);  display.print("12");
    display.setCursor(90, 46);  display.print("18");
    display.setCursor(115, 46); display.print("24");

    display.setCursor(0, 56);
    display.print("A:>  B:<   C:Set");
    display.display();

    if (_redTouch) { beep(); openHour++; if (openHour > 23) openHour = 0; delay(200); }
    if (_greenTouch) { beep(); openHour--; if (openHour < 0) openHour = 23; delay(200); }
    if (_blueTouch) { beep(); delay(200); _endstate = true; EditCloseHour(); }
    delay(50);
  }
}

void Tumbly::EditCloseHour() {
  _endstate = false;
  while (!_endstate) {
    readButtons();
    display.clearDisplay();

    display.setCursor(0, 0);
    display.println("Set Close Time");
    display.setCursor(0, 10);
    display.setTextSize(2);
    if (closeHour < 10) display.print('0');
    display.print(closeHour);
    display.print(":00");
    display.setTextSize(1);

    int ox = (openHour * 128) / 24;
    int cx = (closeHour * 128) / 24;

    display.setCursor(constrain(cx - 3, 0, 121), 28);
    display.print("v");

    display.drawRect(0, 36, 128, 6, SH110X_WHITE);

    if (openHour < closeHour) {
      display.fillRect(ox, 37, cx - ox, 4, SH110X_WHITE);
    } else if (openHour > closeHour) {
      display.fillRect(ox, 37, 128 - ox, 4, SH110X_WHITE);
      display.fillRect(0, 37, cx, 4, SH110X_WHITE);
    }

    display.drawFastVLine(ox, 33, 3, SH110X_WHITE);

    for (int i = 0; i <= 4; i++) {
      int tx = constrain(i * 32, 0, 127);
      display.drawFastVLine(tx, 42, 3, SH110X_WHITE);
    }
    display.setCursor(0, 46);   display.print("0");
    display.setCursor(29, 46);  display.print("6");
    display.setCursor(58, 46);  display.print("12");
    display.setCursor(90, 46);  display.print("18");
    display.setCursor(115, 46); display.print("24");

    display.setCursor(0, 56);
    display.print("A:>  B:<   C:Set");
    display.display();

    if (_redTouch) { beep(); closeHour++; if (closeHour > 23) closeHour = 0; delay(200); }
    if (_greenTouch) { beep(); closeHour--; if (closeHour < 0) closeHour = 23; delay(200); }
    if (_blueTouch) { beep(); delay(200); _endstate = true; EditOpenPosition(); }
    delay(50);
  }
}

void Tumbly::EditOpenPosition() {
  _endstate = false;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Open Position");
  display.setCursor(0, 12);
  display.println("Servo moves to this");
  display.println("when door opens");
  display.setCursor(0, 40);
  display.println("Adjust & watch!");
  display.setCursor(0, 54);
  display.println("A+  B-  C>Next");
  display.display();
  delay(1500);

  digitalWrite(11, HIGH);
  myservo.attach(10);
  myservo.write(openpos);
  delay(400);
  myservo.detach();
  digitalWrite(11, LOW);

  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Open Position");
    display.setCursor(0, 12);
    display.setTextSize(2);
    display.print(openpos);
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Watch servo move");
    display.setCursor(0, 54);
    display.println("A+  B-  C>Next");
    display.display();

    if (_redTouch) {
      beep(); openpos += 10; if (openpos > 180) openpos = 180;
      digitalWrite(11, HIGH); myservo.attach(10); myservo.write(openpos); delay(400); myservo.detach(); digitalWrite(11, LOW);
      delay(200);
    }
    if (_greenTouch) {
      beep(); openpos -= 10; if (openpos < 0) openpos = 0;
      digitalWrite(11, HIGH); myservo.attach(10); myservo.write(openpos); delay(400); myservo.detach(); digitalWrite(11, LOW);
      delay(200);
    }
    if (_blueTouch) { beep(); delay(200); _endstate = true; EditClosedPosition(); }
    delay(50);
  }
}

void Tumbly::EditClosedPosition() {
  _endstate = false;
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Closed Position");
  display.setCursor(0, 12);
  display.println("Servo moves to this");
  display.println("when door closes");
  display.setCursor(0, 40);
  display.println("Adjust & watch!");
  display.setCursor(0, 54);
  display.println("A+  B-  C>Done");
  display.display();
  delay(1500);

  digitalWrite(11, HIGH);
  myservo.attach(10);
  myservo.write(closedpos);
  delay(400);
  myservo.detach();
  digitalWrite(11, LOW);

  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Closed Position");
    display.setCursor(0, 12);
    display.setTextSize(2);
    display.print(closedpos);
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println("Watch servo move");
    display.setCursor(0, 54);
    display.println("A+  B-  C>Done");
    display.display();

    if (_redTouch) {
      beep(); closedpos += 10; if (closedpos > 180) closedpos = 180;
      digitalWrite(11, HIGH); myservo.attach(10); myservo.write(closedpos); delay(400); myservo.detach(); digitalWrite(11, LOW);
      delay(200);
    }
    if (_greenTouch) {
      beep(); closedpos -= 10; if (closedpos < 0) closedpos = 0;
      digitalWrite(11, HIGH); myservo.attach(10); myservo.write(closedpos); delay(400); myservo.detach(); digitalWrite(11, LOW);
      delay(200);
    }
    if (_blueTouch) {
      beep();
      delay(200);
      _endstate = true;
      saveConfig();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Settings Saved!");
      display.setCursor(0, 12);
      display.print("Opens: ");
      if (openHour < 10) display.print('0');
      display.print(openHour);
      display.print(":00");
      display.setCursor(0, 22);
      display.print("Closes: ");
      if (closeHour < 10) display.print('0');
      display.print(closeHour);
      display.print(":00");
      display.setCursor(0, 34);
      display.print("Open Pos: ");
      display.println(openpos);
      display.setCursor(0, 44);
      display.print("Closed Pos: ");
      display.println(closedpos);
      display.display();
      delay(3000);
    }
    delay(50);
  }
}
