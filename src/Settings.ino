/********************************************************
  Settings Menu
  
  Use buttons to configure:
  - RED: Navigate up/increase value
  - GREEN: Navigate down/decrease value  
  - BLUE: Select/Confirm
  
  Long press RED to enter menu from main screen
********************************************************/

bool _redTouch, _greenTouch, _blueTouch;
unsigned long _menustart;
bool _endstate;

void readButtons() {
  _redTouch = (digitalRead(RED_BUTTON) == LOW);
  _greenTouch = (digitalRead(GREEN_BUTTON) == LOW);
  _blueTouch = (digitalRead(BLUE_BUTTON) == LOW);
}

void beep() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(50);
  digitalWrite(LED_BUILTIN, LOW);
}

/********************************************************
  Main Settings Menu
********************************************************/
void SettingsMenu() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("SETTINGS MENU");
  display.setCursor(0, 12);
  display.println("A: Start");
  display.setCursor(0, 22);
  display.println("C: Edit");
  display.display();
  
  unsigned long menuTimeout = millis();
  
  while (millis() - menuTimeout < 5000) {  // 5 second timeout
    readButtons();
    
    if (_redTouch) {  // Button A
      beep();
      return;  // Exit menu
    }
    
    if (_blueTouch) {  // Button C
      beep();
      delay(200);
      EditOpenHour();
      return;
    }
    delay(50);
  }
}

/********************************************************
  Edit Open Hour
********************************************************/
void EditOpenHour() {
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
    
    if (_redTouch) {
      beep();
      openHour++;
      if (openHour > 23) openHour = 0;
      delay(200);
    }
    
    if (_greenTouch) {
      beep();
      openHour--;
      if (openHour < 0) openHour = 23;
      delay(200);
    }
    
    if (_blueTouch) {
      beep();
      delay(200);
      _endstate = true;
      EditCloseHour();
    }
    
    delay(50);
  }
}

/********************************************************
  Edit Close Hour
********************************************************/
void EditCloseHour() {
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
    
    if (_redTouch) {
      beep();
      closeHour++;
      if (closeHour > 23) closeHour = 0;
      delay(200);
    }
    
    if (_greenTouch) {
      beep();
      closeHour--;
      if (closeHour < 0) closeHour = 23;
      delay(200);
    }
    
    if (_blueTouch) {
      beep();
      delay(200);
      _endstate = true;
      EditOpenPosition();
    }
    
    delay(50);
  }
}

/********************************************************
  Edit Open Position
********************************************************/
void EditOpenPosition() {
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
  
  // Move to current open position
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
    
    if (_redTouch) {
      beep();
      openpos += 5;
      if (openpos > 180) openpos = 180;
      myservo.write(openpos);
      delay(200);
    }
    
    if (_greenTouch) {
      beep();
      openpos -= 5;
      if (openpos < -10) openpos = -10;
      myservo.write(openpos);
      delay(200);
    }
    
    if (_blueTouch) {
      beep();
      digitalWrite(11, LOW);
      myservo.detach();
      delay(200);
      _endstate = true;
      EditClosedPosition();
    }
    
    delay(50);
  }
}

/********************************************************
  Edit Closed Position
********************************************************/
void EditClosedPosition() {
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
  
  // Move to current closed position
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
    
    if (_redTouch) {
      beep();
      closedpos += 5;
      if (closedpos > 180) closedpos = 180;
      myservo.write(closedpos);
      delay(200);
    }
    
    if (_greenTouch) {
      beep();
      closedpos -= 5;
      if (closedpos < -10) closedpos = -10;
      myservo.write(closedpos);
      delay(200);
    }
    
    if (_blueTouch) {
      beep();
      digitalWrite(11, LOW);
      myservo.detach();
      delay(200);
      _endstate = true;
      
      // Show confirmation with times AND positions
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
