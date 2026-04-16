/********************************************************
  Tumbly - Tumble Feeder Library
  Time-restricted feeding with OLED display and SD logging
  Copyright (c) 2024 Lex Kravitz, Mason Barrett
  Released under GPL-3.0
 ********************************************************/

#ifndef TUMBLY_H
#define TUMBLY_H

#include <Arduino.h>
#include <Servo.h>
#include "ArduinoLowPower.h"
#include "RTClib.h"
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define BUTTON_A      9
#define BUTTON_B      6
#define BUTTON_C      5
#define RED_BUTTON    BUTTON_A
#define GREEN_BUTTON  BUTTON_B
#define BLUE_BUTTON   BUTTON_C

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

class Tumbly {
public:
  Tumbly(String& task);

  void begin();
  void run();

  void ReadSensors();
  void TimedDoor();
  void LightControlledDoor();
  void LogData();
  void UpdateDisplay();
  void GoToSleep();
  void open_door();
  void close_door();

  int   deviceId      = 1;
  int   taskIndex     = 0;
  bool  sdPresent     = true;
  int   lux          = 0;
  bool  doorOpen     = true;
  char  filename[30];
  float measuredvbat = 0;
  int   sleeptime    = 10;
  int   openpos      = 0;
  int   closedpos    = 100;
  int   openHour     = 20;
  int   closeHour    = 4;
  String task;

  RTC_DS3231      rtc;
  Servo           myservo;
  File            logfile;
  Adafruit_SH1107 display;
  SdFat           SD;

private:
  static constexpr int _chipSelect = 4;
  bool          _redTouch   = false;
  bool          _greenTouch = false;
  bool          _blueTouch  = false;
  unsigned long _menustart  = 0;
  bool          _endstate   = false;

  void readButtons();
  void beep();
  void SettingsMenu();
  void EditTask();
  void EditDeviceId();
  void EditOpenHour();
  void EditCloseHour();
  void EditOpenPosition();
  void EditClosedPosition();
  void writeHeader();
  void saveConfig();
  bool loadConfig();
  void error();
};

#endif
