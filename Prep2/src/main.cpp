#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Display constants
#define LCDWIDTH 20
#define LCDHEIGHT 4

//-------------------------------
//---------- Constants ----------
//-------------------------------

//Buttons
const int redBtn = 3; //INT 5 //1
const int blueBtn = 2; //INT 4 //2
const int greenBtn = 18; //INT 3 //3
const int blackBtn = 19; //INT 2 //4

//Two-pos switches
const int switch1 = 53; //SWITCH1
const int switch2 = 49; //SWITCH2
const int powerPin = 51; //PP

//Leds
const int redLed = 13;
const int greenLed  = 12;
const int blueLed  = 11;
const int yellowLed = 10;
const int leds[] = {13,12,11,10};

//PIR
const int pir = 29;

//Rotary encoder
const int dt = 27;
const int clk = 26;
const int reBtn = 28;
bool cw = false;

//Pos meter
const int pos = 35;

//RGB Led
const int rgbRed = 44; 
const int rgbGreen = 46;
const int rgbBlue = 45;
const int rgbLeds[] = {44,46,45};

//--------------------------------------
//---------- END OF CONSTANTS ----------
//--------------------------------------







//--------------------------------------
//---------- VARS / OBJECTS ------------
//--------------------------------------

//LCD Type
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystalScroller lcd = LiquidCrystalScroller(0x27,20,4);

//Flag for encoder
bool reBtnPressed = false;

//Toggle Button Flags
bool redBtnToggle = false;
bool blueBtnToggle = false;
bool greenBtnToggle = false;
bool blackBtnToggle = false;

bool reBtnToggle = false;

//Buttons laststates of toggles
bool redBtnLaststate = false;
bool blueBtnLaststate = false;
bool greenBtnLaststate = false;
bool blackBtnLaststate = false;

bool reBtnLaststate = false;

//Flags --> Non-Interrupts / States
int lastSwitch1State = false;
int lastSwitch2State = false;

//Flags change indicators
bool encoderChange = false;
bool pirChange = false;
bool posChange = false;

bool redBtnChanged = false;
bool blueBtnChanged = false;
bool greenBtnChanged = false;
bool blackBtnChanged = false;
bool reBtnChanged = false;

//Switches change indicators
bool switchChange = false;

//Timers
//Debounce delay --> encoder
int btnWaitDelay = 50;
unsigned long reBtnWait = 0;

//Rotary encoder vars
int reBtnPressedlast = 0;
int dtLaststate = 0;
int clkLaststate = 0;
int encoderPos = 0;

int dtNow = 0;
int clkNow = 0;

//PIR vars
int pirLaststate = 0;
int pirState = 0;

//Pos vars
int posLaststate = 0;
int posState = 0;

//----------- Program state -----------
int programState = 0;






//----------- Program 1 vars -----------
int encoderMode = 0;

int redState = 0;
int redLaststate = 0;

int blueState = 0;
int blueLaststate = 0;

int greenState = 0;
int greenLaststate = 0;

unsigned long redWait = 0;
unsigned long greenWait = 0;
unsigned long blueWait = 0;

int rgbLedDelay = 100;

byte arrow[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000
};

byte ball[] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};

//----------- Program 2 vars -----------
String secret = "411231";
String checkPass = "";
unsigned long passWait = 0;
int passDelay = 5000;

bool alarmState = false;
bool alarmLastState = false;

unsigned long alarmWait = 0;
int alarmDelay = 150;
bool alarmLedState = false;

//----------- Program 3 vars -----------

//----------- Program 4 vars -----------







//---------------------------------
//---------- ACTIVE CODE ----------
//---------------------------------

//Functions declaration
void redBtnCallback();
void blueBtnCallback();
void greenBtnCallback();
void blackBtnCallback();
void clearLeds();
void clearRgbLeds();
void checkInter();
int toDec(int, int);

//CallBacks for interrupts --> flags
void redBtnCallback() {
  redBtnToggle = !redBtnToggle;
}

void blueBtnCallback() {
  blueBtnToggle = !blueBtnToggle;
}

void greenBtnCallback() {
  greenBtnToggle = !greenBtnToggle;
}

void blackBtnCallback() {
  blackBtnToggle = !blackBtnToggle;
}

/* ----------------------------------
 * ----------- Functions ------------
 * ---------------------------------- */
//Turn off all leds
void clearLeds() {
  for(int i = 0; i < 4; i++) {
    digitalWrite(leds[i], LOW);
  }
}

//Turn off RGB Led 
void clearRgbLeds() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(rgbLeds[i], LOW);
  }
}
 
//Interrupt check --> Buttons
void checkInter() {
  //ENCODER BUTTON
  reBtnPressed = !digitalRead(reBtn);
  if(reBtnPressed && !reBtnPressedlast && reBtnWait < millis()) {
    reBtnPressed = false;
    reBtnWait = millis() + btnWaitDelay;

		//Toggle logic
		reBtnToggle = !reBtnToggle;
  
		//CODE
	}
	reBtnPressedlast = !digitalRead(reBtn);

  reBtnPressed = false;
}

//Convert to bin
int toDec(int a, int b) {
  return a + 2*b;
}


/* -------------------------------------
 * ---------- PROGRAM METHODS ----------
 * ------------------------------------- */
void programState1() {
  if(Serial.available()) {
    String data = Serial.readString();

    char *pch;
    char dataC[50];

    strcpy(dataC, data.c_str());

    pch = strtok(dataC, ";");
    redState = atoi(pch);
    if(redState > 255)
      redState = 255;
    else if(redState < 0) {
      redState = 0;
    }
    Serial.println("Red led: "+String(redState));

    pch = strtok(NULL, ";");
    greenState = atoi(pch);
    if(greenState > 255)
      greenState = 255;
    else if(greenState < 0) {
      greenState = 0;
    }
    Serial.println("Green led: "+String(greenState));

    pch = strtok(NULL, ";");
    blueState = atoi(pch);
    if(blueState > 255)
      blueState = 255;
    else if(blueState < 0) {
      blueState = 0;
    }
    Serial.println("Blue led: "+String(blueState));
  }

  if(reBtnChanged) {
    if(encoderMode == 0) {
      encoderMode = encoderPos+1;
      lcd.setCursor(0,encoderPos);
      lcd.write(1);
    }
    else {
      encoderPos = encoderMode-1;
      encoderMode = 0;
      lcd.setCursor(0,encoderPos);
      lcd.write(0);
    }
  }

  if(encoderChange) {
    if(encoderPos > 2) {
      encoderPos = 0;
    }
    else if(encoderPos < 0) {
      encoderPos = 2;
    }

    //Cursor things
    if(encoderMode == 0) {
      lcd.setCursor(0,0);
      lcd.print(" ");
      lcd.setCursor(0,1);
      lcd.print(" ");
      lcd.setCursor(0,2);
      lcd.print(" ");
      lcd.setCursor(0,3);
      lcd.print(" ");

      lcd.setCursor(0,encoderPos);
      lcd.write(0);
    }
    else {
      if(encoderMode == 1) {
        if(cw)
          redState++;
        else
          redState--;

        if(redState < 0)
          redState = 255;
        else if(redState > 255)
          redState = 0;
      }

      else if(encoderMode == 2) {
        if(cw)
          greenState++;
        else
          greenState--;

        if(greenState < 0)
          greenState = 255;
        else if(greenState > 255)
          greenState = 0;
      }

      else if(encoderMode == 3) {
        if(cw)
          blueState++;
        else
          blueState--;

        if(blueState < 0)
          blueState = 255;
        else if(blueState > 255)
          blueState = 0;
      }
    }
  }
  
  if(switchChange || redLaststate != redState) {
    redLaststate = redState;

    lcd.setCursor(2,0);
    lcd.print("RED Value: "+String(redState)+"   ");
    analogWrite(rgbRed, redState);
  }
  if(switchChange || greenLaststate != greenState) {
    greenLaststate = greenState;

    lcd.setCursor(2,1);
    lcd.print("GREEN Value: "+String(greenState)+"   ");
    analogWrite(rgbGreen, greenState);
  }
  if(switchChange || blueLaststate != blueState) {
    blueLaststate = blueState;

    lcd.setCursor(2,2);
    lcd.print("BLUE Value: "+String(blueState)+"   ");
    analogWrite(rgbBlue, blueState);
  }
}

void programState2() {
  //Pokud je alarm ON
  if(alarmState) {
    if(alarmWait < millis()) {
      alarmWait = millis() + alarmDelay;

      alarmLedState = !alarmLedState;
      digitalWrite(redLed, alarmLedState);
    }

    //Pokud se alarm zrovna zapl -->
    if(alarmState != alarmLastState || switchChange) {
      alarmLastState = alarmState;
      lcd.setCursor(0,0);
      lcd.print("Alarm ON ");
      lcd.setCursor(0,1);
      lcd.print("ENTER PASSWORD: ");
    }
  }
  else {
    if(alarmState != alarmLastState || switchChange) {
      alarmLastState = alarmState;
      lcd.setCursor(0,0);
      lcd.print("Alarm off");
    }

    alarmLedState = false;
    digitalWrite(redLed, alarmLedState);
  }

  //Zapni alarm na PIR a shock
  if(pirChange || posChange) {
    alarmState = true;
  }

  if(redBtnChanged) {
    checkPass += "1";
    passWait = millis() + passDelay;

    lcd.setCursor(0,2);
    lcd.print(checkPass);
  }
  else if(blueBtnChanged) {
    checkPass += "2";
    passWait = millis() + passDelay;

    lcd.setCursor(0,2);
    lcd.print(checkPass);
  }
  else if(greenBtnChanged) {
    checkPass += "3";
    passWait = millis() + passDelay;

    lcd.setCursor(0,2);
    lcd.print(checkPass);
  }
  else if(blackBtnChanged) {
    checkPass += "4";
    passWait = millis() + passDelay;

    lcd.setCursor(0,2);
    lcd.print(checkPass);
  }

  if(checkPass != "") {
    lcd.setCursor(0,3);
    int cas = (int)round((passWait-millis())/(unsigned long)1000);
    lcd.print(String(cas > 0 ? cas : 0) + "s left.");
  }

  if(passWait < millis() && checkPass != "") {
    if(checkPass == secret) {
      alarmState = false;
      lcd.setCursor(0,1);
      lcd.print("                    ");
    }
    lcd.setCursor(0,2);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("                    ");
    checkPass = "";
    lcd.setCursor(0,2);
    lcd.print(String(alarmState ? "INCORRECT" : "CORRECT")+" PASSWORD");
    delay(1000);
    lcd.setCursor(0,2);
    lcd.print("                    ");
  }
}

void programState3() {

}

void programState4() {

}



/* ----------------------------------
 * ---------- Main methods ----------
 * ---------------------------------- */
void setup() {
  //Setup serial
  Serial.begin(9600);

  //Button init
  pinMode(redBtn, INPUT);
  pinMode(blueBtn, INPUT);
  pinMode(greenBtn, INPUT);
  pinMode(blackBtn, INPUT);

  //Switches init
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);

  //LEDs init
  pinMode(redLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);

  //Interrupts (Buttons) init
  attachInterrupt(digitalPinToInterrupt(redBtn), redBtnCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(blueBtn), blueBtnCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(greenBtn), greenBtnCallback, FALLING);
  attachInterrupt(digitalPinToInterrupt(blackBtn), blackBtnCallback, FALLING);

  //Rotary encoder
  pinMode(dt, INPUT);
  pinMode(clk, INPUT);
  pinMode(reBtn, INPUT_PULLUP);

  //PIR
  pinMode(pir, INPUT);

  //RGB LED
  pinMode(rgbRed, OUTPUT);
  pinMode(rgbGreen, OUTPUT);
  pinMode(rgbBlue, OUTPUT);

  //POS
  pinMode(pos, INPUT_PULLUP);

  //Setup indicator
  switchChange = true;

  //LCD init
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, arrow);
  lcd.createChar(1, ball);
}

void loop() {
  //Check buttons and interrupts
  checkInter();

  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    lastSwitch1State = switch1State;
    lastSwitch2State = switch2State;
    programState = toDec(switch1State, switch2State);

    switchChange = true;

    lcd.clear();
    clearLeds();
    clearRgbLeds();
  } 

  //Rotary encoder logic
  dtNow = digitalRead(dt);
  clkNow = digitalRead(clk);
  if(dtNow != dtLaststate) {
    encoderChange = true;

    dtLaststate = dtNow;
    if(clkNow == dtNow) {
      encoderPos++;
      cw = true;
    }
    else {
      encoderPos--;
      cw = false;
    }
  }

  //PIR Logic
  pirState = digitalRead(pir);
  if(pirState != pirLaststate) {
    pirChange = true;

    pirLaststate = pirState;
  }

  //Shock sensor
  posState = digitalRead(pos);
  if(posState != posLaststate) {
    posChange = true;

    posLaststate = posState;
  }

	//Buttons check
	if(redBtnToggle != redBtnLaststate) {
		redBtnLaststate = redBtnToggle;
		redBtnChanged = true;
	}
	if(blueBtnToggle != blueBtnLaststate) {
		blueBtnLaststate = blueBtnToggle;
		blueBtnChanged = true;
	}
	if(greenBtnToggle != greenBtnLaststate) {
		greenBtnLaststate = greenBtnToggle;
		greenBtnChanged = true;
	}
	if(blackBtnToggle != blackBtnLaststate) {
		blackBtnLaststate = blackBtnToggle;
		blackBtnChanged = true;
	}
	if(reBtnToggle != reBtnLaststate) {
		reBtnLaststate = reBtnToggle;
		reBtnChanged = true;
	}


  /* ----------------------------------
  * ---------- MAIN LOGIC ------------
  * ---------------------------------- */
  if(programState == 0) {
    programState1();
  }
  else if(programState == 1) {
    programState2();
  }
  else if(programState == 2) {
    programState3();
  }
  else if(programState == 3) {
    programState4();
  }


  //Changed vars
  encoderChange = false;
  pirChange = false;
  posChange = false;

	redBtnChanged = false;
	blueBtnChanged = false;
	greenBtnChanged = false;
	blackBtnChanged = false;
	reBtnChanged = false;

  switchChange = false;
}