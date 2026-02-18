/********************************************************
  Include libraries
********************************************************/
#include <Arduino.h>
#include <Servo.h>
#include "ArduinoLowPower.h"
#include "RTClib.h"
#include <SdFat.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

SdFat SD;  //Make SdFat work with standard SD.h sketches

/********************************************************
  Set variables
********************************************************/
int CAS = 1;  //This is the unique # of the device
int lux = 0;
bool doorOpen = true;
char filename[30];  // make a "char" type variable called "filename" [13]
float measuredvbat;
int sleeptime = 10;  //sleep time in seconds (this sets the interval between sensor readings and logging)

// Door settings (adjustable in menu)
int openpos = -10;    // Servo position when door is open
int closedpos = 100;  // Servo position when door is closed
int openHour = 20;    // Hour to open door (24-hour format, default 8pm)
int closeHour = 4;    // Hour to close door (24-hour format, default 4am)

// OLED Featherwing built-in button pins
#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

// Map to friendly names for settings menu
#define RED_BUTTON    BUTTON_A  // Top button
#define GREEN_BUTTON  BUTTON_B  // Middle button
#define BLUE_BUTTON   BUTTON_C  // Bottom button

/********************************************************
  Setup RTC
********************************************************/
RTC_DS3231 rtc;

/********************************************************
  Setup Servo
********************************************************/
Servo myservo;  // create servo object to control a servo

/********************************************************
  Setup SD Card
********************************************************/
const int chipSelect = 4;
File logfile;  // Create file object

/********************************************************
  Setup OLED Display (Featherwing with SH1107)
  128x64 OLED
********************************************************/
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_HEIGHT, SCREEN_WIDTH, &Wire);