/********************************************************
  Tumbly - Library
  Time-restricted feeding with OLED display and SD logging
  Copyright (c) 2024 Lex Kravitz, Mason Barrett
  Released under GPL-3.0
 ********************************************************/

#ifndef TUMBLY_H
#define TUMBLY_H

/*==============================================================================
  DEPENDENCIES
  Target hardware: Adafruit Feather M0 (SAMD21) + OLED FeatherWing (SH1107),
  DS3231 RTC, microSD, and a feedback (analog-position) hobby servo.
==============================================================================*/

#include <Arduino.h>
#include <Servo.h>
#include "ArduinoLowPower.h"   // LowPower.sleep + wake-on-button interrupt
#include "RTClib.h"            // DS3231 real-time clock
#include <SdFat.h>             // SD logging + config file
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>   // 128x64 OLED

/*==============================================================================
  PIN MAP & DISPLAY CONSTANTS
  The three front buttons are referred to two ways in the code: by physical
  letter (A/B/C) and by the on-screen color convention (RED/GREEN/BLUE).
==============================================================================*/

#define BUTTON_A      9
#define BUTTON_B      6
#define BUTTON_C      5
#define RED_BUTTON    BUTTON_A   // increment / start / "A"
#define GREEN_BUTTON  BUTTON_B   // decrement / resume / "B"
#define BLUE_BUTTON   BUTTON_C   // select / edit / next / "C"
#define SERVO_FEEDBACK A0        // analog position feedback from the servo pot

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64

class Tumbly {
public:
  /*----------------------------------------------------------------------------
    LIFECYCLE
    begin() runs once (calibration, config, animation). run() is the main loop
    body: one wake cycle (sensors -> door logic -> log -> display -> sleep).
  ----------------------------------------------------------------------------*/
  Tumbly(String& task, bool darkMode = false);

  void begin();
  void run();

  /*----------------------------------------------------------------------------
    OPERATIONAL STEPS (called from run(), exposed for flexibility)
  ----------------------------------------------------------------------------*/
  void ReadSensors();          // light + battery
  void TimedDoor();            // open/close by time-of-day window (task 0)
  void LightControlledDoor();  // open/close by ambient light
  void LogData();              // append one CSV row to the SD log
  void UpdateDisplay();        // draw the status screen
  void GoToSleep();            // low-power sleep until next cycle / button wake
  void open_door();            // drive servo to the OPEN feedback target
  void close_door();           // drive servo to the CLOSED feedback target

  /*----------------------------------------------------------------------------
    USER CONFIG (persisted to config.txt)
  ----------------------------------------------------------------------------*/
  int   deviceId      = 1;
  int   taskIndex     = 0;     // 0 TimedDoor, 1 FreeFeeding, 2 Demo
  int   openHour        = 20;
  int   closeHour       = 4;
  int   openMinute      = 0;
  int   closeMinute     = 0;
  int   openpos      = 0;      // legacy position fields (kept for config compat)
  int   closedpos    = 0;
  int   feedbackOpen    = -1;  // ADC target for the OPEN position (-1 = uncalibrated)
  int   feedbackClosed  = -1;  // ADC target for the CLOSED position
  int   sleeptime    = 30;     // seconds per wake cycle

  /*----------------------------------------------------------------------------
    RUNTIME STATE
  ----------------------------------------------------------------------------*/
  bool  sdPresent     = true;
  int   lux          = 0;
  bool  doorOpen     = true;
  char  filename[30];
  float measuredvbat = 0;
  int   feedbackTolerance = 50;  // +/- ADC counts still considered "on target"
  int   lastFeedback    = -1;    // -1 means "not measured this cycle"
  bool  servoError      = false;
  String lastError      = "OK";
  bool   demoMode       = false;
  bool   darkMode       = false;
  String task;

  /*----------------------------------------------------------------------------
    HARDWARE OBJECTS
  ----------------------------------------------------------------------------*/
  RTC_DS3231      rtc;
  Servo           myservo;
  File            logfile;
  Adafruit_SH1107 display;
  SdFat           SD;

private:
  static constexpr int _chipSelect = 4;

  // Debounced button state, refreshed by readButtons().
  bool          _redTouch   = false;
  bool          _greenTouch = false;
  bool          _blueTouch  = false;
  unsigned long _menustart  = 0;
  bool          _endstate   = false;  // signals an Edit* screen to exit

  /*----------------------------------------------------------------------------
    INTERNAL HELPERS
  ----------------------------------------------------------------------------*/
  void readButtons();
  void beep();

  // Settings menu + calibration screens
  void SettingsMenu();
  void EditTask();
  void EditDeviceId();
  void EditOpenHour();
  void EditCloseHour();
  void EditOpenPosition();
  void EditClosedPosition();

  // SD helpers
  void writeHeader();
  void saveConfig();
  bool loadConfig();
  void error();

  // Servo feedback + position verification
  int  readFeedback();
  void HourlyCheck();
  void shakeServo();
  void FatalServoError();

  /*----------------------------------------------------------------------------
    INTERNAL STATE (not user-configurable)
  ----------------------------------------------------------------------------*/
  int  _wakeCount       = 0;
  int  _lastPWM         = 90;   // last commanded servo angle
  int  _openPWM         = 90;   // servo angle that last matched feedbackOpen
  int  _closedPWM       = 90;   // servo angle that last matched feedbackClosed
  int  _demoPhaseCount  = 0;
  bool _demoClosedPhase = true;
  bool _darkActive      = false;  // display temporarily woken in dark mode
  bool _fatalActive     = false;  // latched servo-error state (resume with B)
};

#endif
