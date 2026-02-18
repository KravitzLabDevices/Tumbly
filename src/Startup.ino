void StartUpCommands() {
  /********************************************************
    Start serial connection (this will disconnect after the first sleep)
  ********************************************************/
  Serial.begin(115200);
  Serial.println("Starting up...  Welcome to Mini Tumbler World");

  /********************************************************
    Initialize buttons
  ********************************************************/
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);

  /********************************************************
    Start OLED Display
  ********************************************************/
  if(!display.begin(0x3C, true)) {  // SH1107 at address 0x3C
    Serial.println(F("SH1107 allocation failed"));
    // Continue anyway - program will work without display
  }
  
  display.setRotation(1);  // Rotate display 90 degrees
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

  /********************************************************
    Start servo
  ********************************************************/
  myservo.attach(10);            // attaches the servo on pin A0 to the servo object
  pinMode(8, OUTPUT);            // GREEN LED
  pinMode(11, OUTPUT);           // SERVO POWER
  pinMode(LED_BUILTIN, OUTPUT);  // RED LED

  /********************************************************
    Start RTC
  ********************************************************/
  rtc.begin();

  /********************************************************
    Start SD card, create Data File for current session
  ********************************************************/
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("SD CARD ERROR!");
    display.display();
    delay(2000);
  }

  Serial.println("card initialized.");

  /********************************************************
    Generate a unique filename
    // (code below from: https://forum.arduino.cc/index.php?topic=372248.0)
  ********************************************************/
  int n = 0;
  DateTime now = rtc.now();
  snprintf(filename, sizeof(filename), "FEED%03d_%02d%02d%02d_%02d.csv", CAS, now.month(), now.day(), now.year() - 2000, n);
  while (SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "FEED%03d_%02d%02d%02d_%02d.csv", CAS, now.month(), now.day(), now.year() - 2000, n);
  }

  SdFile::dateTimeCallback(dateTime);  //added this line so the files on the SD card have the correct date and time
  Serial.print("New file created: ");
  Serial.println(filename);
  writeHeader();
  Serial.println("Logfile initialized");

  /********************************************************
    Show Settings Menu
  ********************************************************/
  SettingsMenu();
  
  /********************************************************
    Test door movement
  ********************************************************/
  open_door();
  delay(1000);
  close_door();
  
  // Show ready screen
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

//  dateTime function to help put correct dates/time for windows
void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}
