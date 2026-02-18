/********************************************************
  Include libraries - LEGACY: See Tumbly.h for library version
********************************************************/
#include <Arduino.h>
#include <Servo.h>
#include "ArduinoLowPower.h"
#include "RTClib.h"
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

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

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5
#define RED_BUTTON    BUTTON_A
#define GREEN_BUTTON  BUTTON_B
#define BLUE_BUTTON   BUTTON_C

RTC_DS3231 rtc;
Servo myservo;
const int chipSelect = 4;
File logfile;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire);
