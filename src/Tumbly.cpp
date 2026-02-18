/********************************************************
  Tumbly - Tumble Feeder Library
  Copyright (c) 2024 Lex Kravitz, Mason Barrett
  Released under GPL-3.0
 ********************************************************/

#include "Tumbly.h"

SdFat SD;
int CAS = 1;
int lux = 0;
bool doorOpen = true;
char filename[30];
float measuredvbat;
int sleeptime = 10;
int openpos = -10;
int closedpos = 100;
int openHour = 20;
int closeHour = 4;

RTC_DS3231 rtc;
Servo myservo;
const int chipSelect = 4;
File logfile;
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire);

// Settings menu state
static bool _redTouch, _greenTouch, _blueTouch;
static unsigned long _menustart;
static bool _endstate;

void setDeviceId(int id) { CAS = id; }
void setSleepTime(int seconds) { sleeptime = seconds; }
void setDoorTimes(int openHr, int closeHr) { openHour = openHr; closeHour = closeHr; }
void setDoorPositions(int openPos, int closedPos) { openpos = openPos; closedpos = closedPos; }

static void readButtons();
static void beep();
static void SettingsMenu();
static void EditOpenHour();
static void EditCloseHour();
static void EditOpenPosition();
static void EditClosedPosition();
static void writeHeader();
static void error();
void dateTime(uint16_t* date, uint16_t* time);

void begin() {
  Serial.begin(115200);
  Serial.println("Starting up...  Welcome to Mini Tumbler World");

  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);

  if(!display.begin(0x3C, true)) {
    Serial.println(F("SH1107 allocation failed"));
  }
  display.setRotation(1);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("SimpleCastle");
  display.setCursor(0, 10);
  display.print("Device: CAS");
  display.println(CAS);
  display.setCursor(0, 20);
  display.println("Initializing...");
  display.display();
  delay(2000);

  myservo.attach(10);
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  rtc.begin();

  if (!SD.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("SD CARD ERROR!");
    display.display();
    delay(2000);
  }
  Serial.println("card initialized.");

  int n = 0;
  DateTime now = rtc.now();
  snprintf(filename, sizeof(filename), "FEED%03d_%02d%02d%02d_%02d.csv", CAS, now.month(), now.day(), now.year() - 2000, n);
  while (SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "FEED%03d_%02d%02d%02d_%02d.csv", CAS, now.month(), now.day(), now.year() - 2000, n);
  }

  SdFile::dateTimeCallback(dateTime);
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
  display.display();
  delay(2000);
}

void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  *date = FAT_DATE(now.year(), now.month(), now.day());
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

void run() {
  ReadSensors();
  TimedDoor();
  LogData();
  UpdateDisplay();
  GoToSleep();
}

void ReadSensors() {
  lux = analogRead(A3);
  Serial.print("Lux: ");
  Serial.println(lux);
  analogReadResolution(10);
  measuredvbat = analogRead(A7);
  measuredvbat *= 2;
  measuredvbat *= 3.3;
  measuredvbat /= 1024;
}

void LightControlledDoor() {
  if (lux > 5 && doorOpen == false) open_door();
  else if (lux < 6 && doorOpen == true) close_door();
}

void TimedDoor() {
  rtc.begin();
  DateTime now = rtc.now();
  Serial.print("Hour: ");
  Serial.println(now.hour());

  if (now.year() > 2020) {
    bool inOpenWindow;
    if (openHour > closeHour) {
      inOpenWindow = (now.hour() >= openHour || now.hour() <= closeHour);
    } else {
      inOpenWindow = (now.hour() >= openHour && now.hour() <= closeHour);
    }
    if (inOpenWindow) {
      if (doorOpen == false) open_door();
    } else if (doorOpen == true) {
      close_door();
    }
  }
}

void open_door() {
  digitalWrite(11, HIGH);
  myservo.attach(10);
  for (int pos = closedpos; pos >= openpos; pos--) {
    myservo.write(pos);
    delay(30);
  }
  digitalWrite(11, LOW);
  doorOpen = true;
}

void close_door() {
  digitalWrite(11, HIGH);
  myservo.attach(10);
  for (int pos = openpos; pos <= closedpos; pos++) {
    myservo.write(pos);
    delay(30);
  }
  digitalWrite(11, LOW);
  doorOpen = false;
}

void UpdateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("CAS");
  display.println(CAS);
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
  display.print("Light: ");
  display.print(lux);
  display.setCursor(0, 42);
  display.print("Battery: ");
  display.print(measuredvbat, 2);
  display.print("V");
  display.setCursor(0, 54);
  display.print("Door: ");
  if (doorOpen) display.print("OPEN");
  else display.print("CLOSED");
  display.display();
  delay(1000);
}

void writeHeader() {
  logfile = SD.open(filename, FILE_WRITE);
  logfile.println("Datetime,Device_Number,Battery_Voltage,Light,DoorOpen");
  logfile.close();
}

void LogData() {
  if (!SD.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    error();
  }
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
  logfile.print(CAS);
  logfile.print(",");
  logfile.print(measuredvbat);
  logfile.print(",");
  logfile.print(lux);
  logfile.print(",");
  logfile.println(doorOpen);
  if (!logfile) error();
  logfile.close();
  digitalWrite(8, LOW);
}

void error() {
  digitalWrite(13, HIGH);
  digitalWrite(8, HIGH);
}

void GoToSleep() {
  Serial.print("Sleeping...");
  digitalWrite(LED_BUILTIN, LOW);
  display.oled_command(SH110X_DISPLAYOFF);
  LowPower.sleep(sleeptime * 1000);
  Serial.println("Awake!");
  display.oled_command(SH110X_DISPLAYON);
  digitalWrite(LED_BUILTIN, HIGH);
}

static void readButtons() {
  _redTouch = (digitalRead(RED_BUTTON) == LOW);
  _greenTouch = (digitalRead(GREEN_BUTTON) == LOW);
  _blueTouch = (digitalRead(BLUE_BUTTON) == LOW);
}

static void beep() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
}

static void SettingsMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SETTINGS MENU");
  display.setCursor(0, 12);
  display.println("A: Start");
  display.setCursor(0, 22);
  display.println("C: Edit");
  display.display();

  unsigned long menuTimeout = millis();
  while (millis() - menuTimeout < 5000) {
    readButtons();
    if (_redTouch) { beep(); return; }
    if (_blueTouch) { beep(); delay(200); EditOpenHour(); return; }
    delay(50);
  }
}

static void EditOpenHour() {
  _endstate = false;
  _menustart = millis();
  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Open Time");
    display.setCursor(0, 12);
    display.setTextSize(2);
    if (openHour < 10) display.print('0');
    display.print(openHour);
    display.print(":00");
    display.setTextSize(1);
    display.setCursor(0, 32);
    display.println("Door opens at this");
    display.println("time each day");
    display.setCursor(0, 54);
    display.println("A+  B-  C>Next");
    display.display();

    if (_redTouch) { beep(); openHour++; if (openHour > 23) openHour = 0; delay(200); }
    if (_greenTouch) { beep(); openHour--; if (openHour < 0) openHour = 23; delay(200); }
    if (_blueTouch) { beep(); delay(200); _endstate = true; EditCloseHour(); }
    delay(50);
  }
}

static void EditCloseHour() {
  _endstate = false;
  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Close Time");
    display.setCursor(0, 12);
    display.setTextSize(2);
    if (closeHour < 10) display.print('0');
    display.print(closeHour);
    display.print(":00");
    display.setTextSize(1);
    display.setCursor(0, 32);
    display.println("Door closes at this");
    display.println("time each day");
    display.setCursor(0, 54);
    display.println("A+  B-  C>Next");
    display.display();

    if (_redTouch) { beep(); closeHour++; if (closeHour > 23) closeHour = 0; delay(200); }
    if (_greenTouch) { beep(); closeHour--; if (closeHour < 0) closeHour = 23; delay(200); }
    if (_blueTouch) { beep(); delay(200); _endstate = true; EditOpenPosition(); }
    delay(50);
  }
}

static void EditOpenPosition() {
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
  delay(300);

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

    if (_redTouch) { beep(); openpos += 5; if (openpos > 180) openpos = 180; myservo.write(openpos); delay(200); }
    if (_greenTouch) { beep(); openpos -= 5; if (openpos < -10) openpos = -10; myservo.write(openpos); delay(200); }
    if (_blueTouch) { beep(); digitalWrite(11, LOW); myservo.detach(); delay(200); _endstate = true; EditClosedPosition(); }
    delay(50);
  }
}

static void EditClosedPosition() {
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
  delay(300);

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

    if (_redTouch) { beep(); closedpos += 5; if (closedpos > 180) closedpos = 180; myservo.write(closedpos); delay(200); }
    if (_greenTouch) { beep(); closedpos -= 5; if (closedpos < -10) closedpos = -10; myservo.write(closedpos); delay(200); }
    if (_blueTouch) {
      beep();
      digitalWrite(11, LOW);
      myservo.detach();
      delay(200);
      _endstate = true;
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
