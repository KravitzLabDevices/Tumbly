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

extern SdFat SD;

// Configurable variables (call setDeviceId, setDoorTimes, etc. before begin())
extern int CAS;
extern int lux;
extern bool doorOpen;
extern char filename[30];
extern float measuredvbat;
extern int sleeptime;
extern int openpos;
extern int closedpos;
extern int openHour;
extern int closeHour;

// OLED Featherwing button pins
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define RED_BUTTON    BUTTON_A
#define GREEN_BUTTON  BUTTON_B
#define BLUE_BUTTON   BUTTON_C

// Hardware objects (initialized by begin())
extern RTC_DS3231 rtc;
extern Servo myservo;
extern const int chipSelect;
extern File logfile;
extern Adafruit_SH1107 display;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Configuration (call before begin())
void setDeviceId(int id);
void setSleepTime(int seconds);
void setDoorTimes(int openHr, int closeHr);
void setDoorPositions(int openPos, int closedPos);

// Main API
void begin();           // Call from setup() - initializes all hardware
void run();             // Call from loop() - runs one iteration (sensors, door, log, display, sleep)

// Individual operations (for custom sketches)
void ReadSensors();
void TimedDoor();
void LightControlledDoor();
void LogData();
void UpdateDisplay();
void GoToSleep();
void open_door();
void close_door();

#endif
