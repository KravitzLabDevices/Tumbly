/********************************************************
  Operate door based on light levels
********************************************************/
void LightControlledDoor() {
  //if it's dark
  if (lux > 5 and doorOpen == false) {
    open_door();
  }

  //if it's light
  else if (lux < 6 and doorOpen == true) {
    close_door();
  }
}

/********************************************************
  Operate door based on hour between openHour and closeHour
********************************************************/
void TimedDoor() {
  rtc.begin();  //restart rtc
  DateTime now = rtc.now();
  Serial.print("Hour: ");
  Serial.println(now.hour());

  if (now.year() > 2020) {  // if there's an issue with the RTC it will return 2000.  If that happens let's ignore it and try again on the next loop.
    // Check if we're in the "open" time window
    // Handle wrap-around (e.g., 8pm to 4am crosses midnight)
    bool inOpenWindow;
    if (openHour > closeHour) {
      // Wraps around midnight (e.g., 20:00 to 4:00)
      inOpenWindow = (now.hour() >= openHour || now.hour() <= closeHour);
    } else {
      // Doesn't wrap (e.g., 8:00 to 16:00)
      inOpenWindow = (now.hour() >= openHour && now.hour() <= closeHour);
    }
    
    if (inOpenWindow) {
      if (doorOpen == false) {
        open_door();
      }
    } else if (doorOpen == true) {
      close_door();
    }
  }
}

/********************************************************
  Open door
********************************************************/
void open_door() {
  digitalWrite(11, HIGH);
  myservo.attach(10);  // attaches the servo on pin A0 to the servo object

  //smoothly open servo door
  for (int pos = closedpos; pos >= openpos; pos--) {
    myservo.write(pos);  // Move the servo to the current position
    delay(30);           // Wait before moving to the next position
  }

  digitalWrite(11, LOW);
  doorOpen = true;
}

/********************************************************
  Close door
********************************************************/
void close_door() {
  digitalWrite(11, HIGH);
  myservo.attach(10);  // attaches the servo on pin A0 to the servo object

  //smoothly close  servo door
  for (int pos = openpos; pos <= closedpos; pos++) {
    myservo.write(pos);  // Move the servo to the current position
    delay(30);           // Wait before moving to the next position
  }

  digitalWrite(11, LOW);
  doorOpen = false;
}
