/********************************************************
  Tumbly - Tumble Feeder Library
  Copyright (c) 2024 Lex Kravitz, Mason Barrett
  Released under GPL-3.0
 ********************************************************/

#include "Tumbly.h"
#include <math.h>

// ── Startup animation (file-scope, not part of class API) ─────────────────

static void _animFood(Adafruit_SH1107& d) {
  d.fillCircle( 7,62,3,SH110X_WHITE); d.fillCircle(16,63,3,SH110X_WHITE);
  d.fillCircle(25,61,3,SH110X_WHITE); d.fillCircle(34,63,3,SH110X_WHITE);
  d.fillCircle(42,62,3,SH110X_WHITE); d.fillCircle(11,55,3,SH110X_WHITE);
  d.fillCircle(21,54,3,SH110X_WHITE); d.fillCircle(30,56,3,SH110X_WHITE);
  d.fillCircle(39,54,3,SH110X_WHITE); d.fillCircle(15,48,3,SH110X_WHITE);
  d.fillCircle(26,47,3,SH110X_WHITE); d.fillCircle(36,49,2,SH110X_WHITE);
  d.fillCircle(19,41,3,SH110X_WHITE); d.fillCircle(29,40,2,SH110X_WHITE);
  d.fillCircle(24,35,2,SH110X_WHITE);
}

static void _animCell(Adafruit_SH1107& d, int cellY) {
  int boxTop    = max(cellY, 0);
  int boxBottom = min(cellY + 43, 63);
  if (boxBottom < boxTop) return;
  if (cellY >= 0 && cellY <= 61)
    d.fillRect(2, cellY, 46, 3, SH110X_WHITE);
  int barXs[] = {2, 13, 24, 35, 46};
  int barLen  = boxBottom - boxTop + 1;
  for (int i = 0; i < 5; i++) {
    d.drawFastVLine(barXs[i],     boxTop, barLen, SH110X_WHITE);
    d.drawFastVLine(barXs[i] + 1, boxTop, barLen, SH110X_WHITE);
  }
}

static void _animMouse(Adafruit_SH1107& d, int ox) {
  // Body — scanline horizontal ellipse
  { const int ecx=95+ox, ecy=60, ra=17, rb=9;
    for (int y=max(0,ecy-rb); y<=min(63,ecy+rb); y++) {
      float dy=(float)(y-ecy);
      int   dx=(int)(ra*sqrtf(1.0f-(dy*dy)/((float)(rb*rb))));
      d.drawFastHLine(ecx-dx, y, 2*dx+1, SH110X_WHITE);
    }
  }
  d.fillCircle(81+ox,50,7,SH110X_WHITE);                          // head
  d.fillCircle(86+ox,43,4,SH110X_WHITE);                          // ear
  d.fillTriangle(68+ox,50,74+ox,46,74+ox,54,SH110X_WHITE);        // nose
  d.fillCircle(82+ox,47,2,SH110X_BLACK);                          // eye
  d.drawLine(71+ox,49,63+ox,45,SH110X_WHITE);                     // whiskers
  d.drawLine(71+ox,51,62+ox,51,SH110X_WHITE);
  d.drawLine(71+ox,53,63+ox,57,SH110X_WHITE);
  { const int tcx=115+ox, tcy=59, tr=9;                           // tail arc
    for (int a=180; a>0; a-=15) {
      float r1=a*3.14159265f/180.0f, r2=(a-15)*3.14159265f/180.0f;
      d.drawLine(tcx+(int)(tr*cosf(r1)+0.5f), tcy-(int)(tr*sinf(r1)+0.5f),
                 tcx+(int)(tr*cosf(r2)+0.5f), tcy-(int)(tr*sinf(r2)+0.5f),
                 SH110X_WHITE);
    }
  }
}

static void _animTumbly(Adafruit_SH1107& d, GFXcanvas1& canvas, int startX) {
  if (startX >= 128) return;
  for (int cy=0; cy<20; cy++) {
    int lean=(5*max(0,16-cy))/16;
    for (int cx=0; cx<96; cx++) {
      if (canvas.getPixel(cx,cy)) {
        int dx=startX+cx+lean;
        if (dx>=0 && dx<128) d.drawPixel(dx, cy, SH110X_WHITE);
      }
    }
  }
  int ls=max(0,startX);
  d.drawFastHLine(ls,  0, 128-ls, SH110X_WHITE);
  d.drawFastHLine(ls, 20, 128-ls, SH110X_WHITE);
}

static void _animFrame(Adafruit_SH1107& d, GFXcanvas1& canvas, int tx, int cy, int mx) {
  d.clearDisplay();
  _animFood(d); _animCell(d,cy); _animMouse(d,mx); _animTumbly(d,canvas,tx);
  d.display();
}

static void _animPlay(Adafruit_SH1107& d) {
  GFXcanvas1 canvas(96, 20);
  canvas.fillScreen(0); canvas.setTextSize(2);
  canvas.setTextColor(1); canvas.setCursor(2,2);
  canvas.print("TUMBLY");
  delay(100);

  _animFrame(d,canvas,128,-50,128); delay(300);
  _animFrame(d,canvas, 95,-50,128); delay(350);
  _animFrame(d,canvas, 75,-50,128); delay(350);
  _animFrame(d,canvas, 62,-50,128); delay(350);
  _animFrame(d,canvas, 50,-50,128); delay(300);

  _animFrame(d,canvas,50,-50, 50); delay(115);
  _animFrame(d,canvas,50,-41, 25); delay(100);
  _animFrame(d,canvas,50,-29,  0); delay(80);
  _animFrame(d,canvas,50,-14,  0); delay(60);
  _animFrame(d,canvas,50,  2,  0); delay(42);
  _animFrame(d,canvas,50, 13,  0); delay(30);
  _animFrame(d,canvas,50, 19,  0); delay(22);
  _animFrame(d,canvas,50, 21,  0); delay(18);  // slam!
  _animFrame(d,canvas,50, 14,  0); delay(65);  // bounce
  _animFrame(d,canvas,50, 21,  0); delay(55);
  _animFrame(d,canvas,50, 17,  0); delay(50);
  _animFrame(d,canvas,50, 21,  0); delay(800); // hold
}

// ──────────────────────────────────────────────────────────────────────────

static Tumbly* _instance = nullptr;
static bool _wokenByButton = false;
static void wakeupCallback() { _wokenByButton = true; }

static void _dateTimeCallback(uint16_t* date, uint16_t* time) {
  DateTime now = _instance->rtc.now();
  *date = FAT_DATE(now.year(), now.month(), now.day());
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

Tumbly::Tumbly(String& task, bool darkMode) : display(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire), task(task), darkMode(darkMode) {}

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
  display.oled_command(SH110X_DISPLAYOFF); // blank screen while GRAM is garbage
  display.setRotation(1);
  display.clearDisplay();
  display.display();
  display.oled_command(SH110X_DISPLAYON);
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  _animPlay(display);
  display.clearDisplay();
  display.display();

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
  if (task == "Demo") demoMode = true;
  if (demoMode) sleeptime = 5;

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
  lastFeedback = -1;
  lastError    = "OK";
  _wakeCount++;
  ReadSensors();
  if (demoMode) {
    _demoPhaseCount++;
    if (_demoPhaseCount > 5) {
      _demoPhaseCount = 1;
      _demoClosedPhase = !_demoClosedPhase;
    }
    if (_demoClosedPhase) {
      if (doorOpen) close_door();
    } else {
      if (!doorOpen) open_door();
    }
    if (_wakeCount >= 2) {
      _wakeCount = 0;
      HourlyCheck();
    }
  } else {
    if (taskIndex == 0) {
      TimedDoor();
    } else if (taskIndex == 1) {
      if (!doorOpen) open_door();
    }
    if (_wakeCount >= (300 / max(sleeptime, 1))) {
      _wakeCount = 0;
      HourlyCheck();
    }
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
  if (feedbackOpen <= 0) return;
  digitalWrite(11, HIGH);
  myservo.attach(10, 500, 2500);
  delay(50);
  int step = (feedbackOpen > feedbackClosed) ? 1 : -1;
  int pos = _lastPWM;
  for (; pos >= 0 && pos <= 180; pos += step) {
    myservo.write(pos);
    delay(40);
    if (abs(analogRead(SERVO_FEEDBACK) - feedbackOpen) <= feedbackTolerance) break;
  }
  if (pos < 0 || pos > 180) {
    pos = (step > 0) ? 0 : 180;
    for (; pos >= 0 && pos <= 180; pos += step) {
      myservo.write(pos);
      delay(40);
      if (abs(analogRead(SERVO_FEEDBACK) - feedbackOpen) <= feedbackTolerance) break;
    }
  }
  _lastPWM = constrain(pos, 0, 180);
  _openPWM = _lastPWM;
  myservo.detach();
  digitalWrite(11, LOW);
  doorOpen = true;
}

void Tumbly::close_door() {
  if (feedbackClosed <= 0) return;
  digitalWrite(11, HIGH);
  myservo.attach(10, 500, 2500);
  delay(50);
  int step = (feedbackClosed > feedbackOpen) ? 1 : -1;
  int pos = _lastPWM;
  for (; pos >= 0 && pos <= 180; pos += step) {
    myservo.write(pos);
    delay(40);
    if (abs(analogRead(SERVO_FEEDBACK) - feedbackClosed) <= feedbackTolerance) break;
  }
  if (pos < 0 || pos > 180) {
    pos = (step > 0) ? 0 : 180;
    for (; pos >= 0 && pos <= 180; pos += step) {
      myservo.write(pos);
      delay(40);
      if (abs(analogRead(SERVO_FEEDBACK) - feedbackClosed) <= feedbackTolerance) break;
    }
  }
  _lastPWM = constrain(pos, 0, 180);
  _closedPWM = _lastPWM;
  myservo.detach();
  digitalWrite(11, LOW);
  doorOpen = false;
}

void Tumbly::UpdateDisplay() {
  if (darkMode && !_darkActive) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print("Dev ");
  display.println(deviceId);
  display.setTextSize(1);

  // Battery indicator - top right
  int bars = constrain((int)((measuredvbat - 3.5f) / 0.175f), 0, 4);
  display.setCursor(74, 5);
  display.print(measuredvbat, 2);
  display.print("V");
  display.drawRect(105, 1, 20, 14, SH110X_WHITE);  // battery body
  display.fillRect(125, 5, 3, 6, SH110X_WHITE);     // positive terminal nub
  for (int i = 0; i < bars; i++) {
    display.fillRect(107 + i * 4, 3, 3, 10, SH110X_WHITE);
  }

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
  logfile.println("Datetime,Device_Number,Task,Battery_Voltage,Light Sensor,DoorOpen,Servo_Feedback,Error");
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
  cfg.println(feedbackOpen);
  cfg.println(feedbackClosed);
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
  sleeptime      = cfg.parseInt();
  feedbackOpen   = cfg.parseInt();
  feedbackClosed = cfg.parseInt();
  cfg.close();
  const char* tasks[] = {"TimedDoor", "FreeFeeding", "Demo"};
  if (taskIndex >= 0 && taskIndex < 3) task = tasks[taskIndex];
  return true;
}

void Tumbly::LogData() {
  if (!SD.begin(_chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    sdPresent = false;
    return;
  }
  sdPresent = true;
  if (!darkMode || _darkActive) digitalWrite(8, HIGH);
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
  logfile.print(task);
  logfile.print(",");
  logfile.print(measuredvbat);
  logfile.print(",");
  logfile.print(lux > 50 ? "Dark" : "Light");
  logfile.print(",");
  logfile.print(doorOpen);
  logfile.print(",");
  logfile.print(lastFeedback);
  logfile.print(",");
  logfile.println(lastError);
  if (!logfile) error();
  logfile.close();
  if (!darkMode || _darkActive) digitalWrite(8, LOW);
}

void Tumbly::error() {
  digitalWrite(13, HIGH);
  digitalWrite(8, HIGH);
}

int Tumbly::readFeedback() {
  digitalWrite(11, HIGH);
  delay(5);
  int val = analogRead(SERVO_FEEDBACK);
  digitalWrite(11, LOW);
  return val;
}

void Tumbly::HourlyCheck() {
  if (feedbackOpen <= 0 || feedbackClosed <= 0) return;

  lastFeedback = readFeedback();
  int expected = demoMode ? (_demoClosedPhase ? feedbackClosed : feedbackOpen)
                           : (doorOpen ? feedbackOpen : feedbackClosed);

  if (abs(lastFeedback - expected) <= feedbackTolerance) {
    lastError = "OK";
    servoError = false;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Hourly check");
    display.setCursor(0, 14);
    display.println("completed,");
    display.setCursor(0, 28);
    display.println("Goodnight!");
    display.display();
    delay(3000);
    return;
  }

  servoError = true;
  bool targetOpen = doorOpen;

  auto tryCorrect = [&]() {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Servo incorrect!");
    display.setCursor(0, 14);
    display.println("Adjusting...");
    display.display();

    int targetFB = targetOpen ? feedbackOpen : feedbackClosed;
    int step = targetOpen ? ((feedbackOpen  > feedbackClosed) ? 1 : -1)
                          : ((feedbackClosed > feedbackOpen)  ? 1 : -1);
    // Estimate current PWM from current feedback so sweep starts where servo actually is
    int startPWM = constrain(
      map(lastFeedback, feedbackClosed, feedbackOpen, _closedPWM, _openPWM),
      0, 180);

    digitalWrite(11, HIGH);
    myservo.attach(10, 500, 2500);
    delay(50);
    int pos = startPWM;
    for (; pos >= 0 && pos <= 180; pos += step) {
      myservo.write(pos);
      delay(40);
      if (abs(analogRead(SERVO_FEEDBACK) - targetFB) <= feedbackTolerance) break;
    }
    if (pos < 0 || pos > 180) {
      pos = (step > 0) ? 0 : 180;
      for (; pos >= 0 && pos <= 180; pos += step) {
        myservo.write(pos);
        delay(40);
        if (abs(analogRead(SERVO_FEEDBACK) - targetFB) <= feedbackTolerance) break;
      }
    }
    _lastPWM = constrain(pos, 0, 180);
    if (targetOpen) _openPWM  = _lastPWM;
    else            _closedPWM = _lastPWM;
    myservo.detach();
    digitalWrite(11, LOW);
    doorOpen = targetOpen;
    lastFeedback = readFeedback();
  };

  // Attempt 1
  lastError = String("POS_ERR:") + lastFeedback + "/" + expected;
  LogData();
  tryCorrect();
  if (abs(lastFeedback - expected) <= feedbackTolerance) {
    lastError = "CORRECTED"; servoError = false; LogData();
    display.clearDisplay(); display.setCursor(0, 0);
    display.println("Position corrected,");
    display.setCursor(0, 14); display.println("Goodnight!");
    display.display(); delay(3000); return;
  }

  // Shake 1 + Attempt 2
  lastError = String("SHAKE1:") + lastFeedback + "/" + expected;
  LogData();
  display.clearDisplay(); display.setCursor(0, 0);
  display.println("Still wrong.");
  display.setCursor(0, 14); display.println("Shaking...");
  display.display();
  shakeServo();
  tryCorrect();
  if (abs(lastFeedback - expected) <= feedbackTolerance) {
    lastError = "CORRECTED"; servoError = false; LogData();
    display.clearDisplay(); display.setCursor(0, 0);
    display.println("Position corrected,");
    display.setCursor(0, 14); display.println("Goodnight!");
    display.display(); delay(3000); return;
  }

  // Shake 2 + Attempt 3
  lastError = String("SHAKE2:") + lastFeedback + "/" + expected;
  LogData();
  display.clearDisplay(); display.setCursor(0, 0);
  display.println("Shaking again...");
  display.display();
  shakeServo();
  tryCorrect();
  if (abs(lastFeedback - expected) <= feedbackTolerance) {
    lastError = "CORRECTED"; servoError = false; LogData();
    display.clearDisplay(); display.setCursor(0, 0);
    display.println("Position corrected,");
    display.setCursor(0, 14); display.println("Goodnight!");
    display.display(); delay(3000); return;
  }

  // All attempts failed
  lastError = String("FATAL:") + lastFeedback + "/" + expected;
  LogData();
  FatalServoError();
}

void Tumbly::shakeServo() {
  // Shake in place — estimate current PWM from last feedback so we don't
  // try to move first (servo may be jammed at the wrong position)
  int center = constrain(
    map(lastFeedback, feedbackClosed, feedbackOpen, _closedPWM, _openPWM),
    0, 180);

  digitalWrite(11, HIGH);
  myservo.attach(10, 500, 2500);
  delay(50);

  unsigned long shakeEnd = millis() + 3000;
  bool toggle = false;
  while (millis() < shakeEnd) {
    myservo.write(toggle ? constrain(center + 10, 0, 180) : constrain(center - 10, 0, 180));
    delay(50);
    toggle = !toggle;
  }
  myservo.write(center);
  delay(200);
  myservo.detach();
  digitalWrite(11, LOW);
}

void Tumbly::FatalServoError() {
  unsigned long waitEnd = millis() + 2000;
  while (digitalRead(RED_BUTTON) == LOW && millis() < waitEnd) delay(50);
  delay(200);
  display.oled_command(SH110X_DISPLAYON);
  while (true) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("!! SERVO ERROR !!");
    display.setCursor(0, 12);
    display.println("Could not correct");
    display.println("position.");
    display.setCursor(0, 32);
    display.println("Manual fix needed.");
    display.setCursor(0, 46);
    display.println("B: Fixed! Resume");
    display.display();
    readButtons();
    if (_greenTouch) {
      beep(); delay(300);
      lastError  = "MANUAL_FIXED";
      servoError = false;
      return;
    }
    delay(100);
  }
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
  _darkActive = _wokenByButton;
  if (!darkMode || _darkActive) {
    display.oled_command(SH110X_DISPLAYON);
    digitalWrite(LED_BUILTIN, HIGH);
  }
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
      if (demoMode) {
        display.print("Device: Demo");
        display.setCursor(0, 10);
        display.print("Task:   Demo");
        display.setCursor(0, 24);
        display.print("Open position:");
        display.print(feedbackOpen  >= 0 ? String(feedbackOpen)  : "--");
        display.setCursor(0, 34);
        display.print("Closed position:");
        display.print(feedbackClosed >= 0 ? String(feedbackClosed) : "--");
      } else {
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
        display.print("Open position:");
        display.print(feedbackOpen  >= 0 ? String(feedbackOpen)  : "--");
        display.setCursor(0, 45);
        display.print("Closed position:");
        display.print(feedbackClosed >= 0 ? String(feedbackClosed) : "--");
      }
      display.setCursor(0, 56);
      display.print("A:Start");
      display.setCursor(74, 56);
      display.print("C:Edit");
      display.display();
      needsRedraw = false;
    }
    readButtons();
    if (_redTouch) { beep(); saveConfig(); return; }
    if (_blueTouch) {
      beep(); delay(200);
      if (demoMode) EditOpenPosition();
      else EditTask();
      needsRedraw = true;
    }
    delay(50);
  }
}

void Tumbly::EditTask() {
  const char* tasks[] = {"TimedDoor", "FreeFeeding", "Demo"};
  const int numTasks = 3;
  _endstate = false;
  while (!_endstate) {
    readButtons();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Select Task");
    display.setCursor(10, 16);
    display.print("TimedDoor");
    if (taskIndex == 0) display.print("  <");
    display.setCursor(10, 28);
    display.print("FreeFeeding");
    if (taskIndex == 1) display.print("  <");
    display.setCursor(10, 40);
    display.print("Demo");
    if (taskIndex == 2) display.print("  <");
    display.setCursor(0, 54);
    display.print("A:Change   C:Select");
    display.display();

    if (_redTouch) { beep(); taskIndex = (taskIndex + 1) % numTasks; delay(200); }
    if (_blueTouch) {
      beep();
      task = tasks[taskIndex];
      demoMode = (task == "Demo");
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
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("== OPEN Position ==");
  display.setCursor(0, 14);
  display.println("Move hopper to the");
  display.println("OPEN position");
  display.println("by hand.");
  display.setCursor(0, 44);
  display.println("Hold it, press C.");
  display.display();

  _endstate = false;
  while (!_endstate) {
    readButtons();
    if (_blueTouch) {
      beep(); delay(200);
      digitalWrite(11, HIGH); delay(10);
      feedbackOpen = analogRead(SERVO_FEEDBACK);
      digitalWrite(11, LOW);
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Open pos locked!");
      display.setCursor(0, 18);
      display.print("Value: ");
      display.println(feedbackOpen);
      display.display();
      delay(1500);
      _endstate = true;
      EditClosedPosition();
    }
    delay(50);
  }
}

void Tumbly::EditClosedPosition() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("== CLOSED Position =");
  display.setCursor(0, 14);
  display.println("Move hopper to the");
  display.println("CLOSED position");
  display.println("by hand.");
  display.setCursor(0, 44);
  display.println("Hold it, press C.");
  display.display();

  _endstate = false;
  while (!_endstate) {
    readButtons();
    if (_blueTouch) {
      beep(); delay(200);
      digitalWrite(11, HIGH); delay(10);
      feedbackClosed = analogRead(SERVO_FEEDBACK);
      digitalWrite(11, LOW);
      _endstate = true;
      saveConfig();
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Settings Saved!");
      display.setCursor(0, 12);
      display.print("Open position: ");
      display.println(feedbackOpen);
      display.setCursor(0, 22);
      display.print("Closed position: ");
      display.println(feedbackClosed);
      display.setCursor(0, 34);
      display.print("Opens: ");
      if (openHour < 10) display.print('0');
      display.print(openHour);
      display.print(":00");
      display.setCursor(0, 44);
      display.print("Closes: ");
      if (closeHour < 10) display.print('0');
      display.print(closeHour);
      display.print(":00");
      display.display();
      delay(3000);
    }
    delay(50);
  }
}
