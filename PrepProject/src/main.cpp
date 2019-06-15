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
const int redBtn = 3; //INT 5
const int blueBtn = 2; //INT 4
const int greenBtn = 18; //INT 3
const int blackBtn = 19; //INT 2

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
const int rgbRed = 32; 
const int rgbGreen = 33;
const int rgbBlue = 34;
const int rgbLeds[] = {32,33,34};

//--------------------------------------
//---------- END OF CONSTANTS ----------
//--------------------------------------

//--------------------------------------
//---------- VARS / OBJECTS ------------
//--------------------------------------

//LCD Type
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystalScroller lcd = LiquidCrystalScroller(0x27,20,4);

//Flags --> Interrupts
bool redBtnPressed = false;
bool blueBtnPressed = false;
bool greenBtnPressed = false;
bool blackBtnPressed = false;

//Flags --> Non-Interrupts / States
bool displayChanged = false; //Indicates change of the display
bool redLedState = false;
bool redLedSwitching = false;

int lastSwitch1State = false;
int lastSwitch2State = false;

//Timers
int btnWaitDelay = 200;
unsigned long redBtnWait = 0;
unsigned long blueBtnWait = 0;
unsigned long greenBtnWait = 0;
unsigned long blackBtnWait = 0;

unsigned long reBtnWait = 0;

unsigned long animWait = 0;
int animDelay = 300;

//Rotary encoder vars
int dtLaststate = 0;
int clkLaststate = 0;
int encoderPos = 0;

//PIR vars
int pirLaststate = 0;

//Pos vars
int posLaststate = 0;

//Program state
int programState = 0;

//Program 2 vars
int animShift = 0;
String rowText = "";

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
  redBtnPressed = true;
}

void blueBtnCallback() {
  blueBtnPressed = true;
}

void greenBtnCallback() {
  greenBtnPressed = true;
}

void blackBtnCallback() {
  blackBtnPressed = true;
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
  //RED BUTTON
  if(redBtnPressed && redBtnWait < millis()) {
    redBtnPressed = false;
    redBtnWait = millis() + btnWaitDelay;
    //CODE

  }

  //BLUE BUTTON
  if(blueBtnPressed && blueBtnWait < millis()) {
    blueBtnPressed = false;
    blueBtnWait = millis() + btnWaitDelay;
    //CODE

  }

  //GREEN BUTTON
  if(greenBtnPressed && greenBtnWait < millis()) {
    greenBtnPressed = false;
    greenBtnWait = millis() + btnWaitDelay;
    //CODE

  }

  //BLACK BUTTON
  if(blackBtnPressed && blackBtnWait < millis()) {
    blackBtnPressed = false;
    blackBtnWait = millis() + btnWaitDelay;
    //CODE

  }

  //ENCODER BUTTON
  int reBtnPressed = !digitalRead(reBtn);
  if(reBtnPressed && reBtnWait < millis()) {
    reBtnPressed = false;
    reBtnWait = millis() + btnWaitDelay;
    //CODE

    if(programState == 0) {
      clearLeds();
      digitalWrite(leds[encoderPos], HIGH);
    }
  }

  redBtnPressed = false;
  blueBtnPressed = false;
  greenBtnPressed = false;
  blackBtnPressed = false;

  reBtnPressed = false;
}

//Convert to bin
int toDec(int a, int b) {
  return a + 2*b;
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
  attachInterrupt(digitalPinToInterrupt(redBtn), redBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(blueBtn), blueBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(greenBtn), greenBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(blackBtn), blackBtnCallback, RISING);

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

  //LCD init
  lcd.init();
  lcd.backlight();
}

void loop() {
  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    displayChanged = true;
    lastSwitch1State = switch1State;
    lastSwitch2State = switch2State;
    programState = toDec(switch1State, switch2State);

    animShift = 0;
    lcd.clear();
  } 

  //Changed vars
  bool encoderChange = false;
  bool pirChange = false;
  bool posChange = false;

  //Rotary encoder logic
  int dtNow = digitalRead(dt);
  int clkNow = digitalRead(clk);
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
  int pirState = digitalRead(pir);
  if(pirState != pirLaststate) {
    pirChange = true;

    pirLaststate = pirState;
  }

  //Shock sensor
  int posState = digitalRead(pos);
  if(posState != posLaststate) {
    posChange = true;

    posLaststate = posState;
  }

  /* ----------------------------------
  * ---------- MAIN LOGIC ------------
  * ---------------------------------- */
  if(programState == 0) {

    //Rotary encoder logic
    if(encoderChange) {
      //Encoder clamp pos
      if(encoderPos > 3) {
        encoderPos = 0;
      }
      else if(encoderPos < 0) {
        encoderPos = 3;
      }

      digitalWrite(rgbRed, cw);
      lcd.setCursor(12,0);
      lcd.print("       ");
      lcd.setCursor(0,0);
      lcd.print("Encoder pos: "+String(encoderPos));
    }

    //PIR Logic
    if(pirChange) {
      lcd.setCursor(0,1);
      lcd.print("          ");
      lcd.setCursor(0,1);
      lcd.print("PIR: ");
      lcd.print(pirState);
      digitalWrite(rgbBlue, pirState);
    }

    //Shock sensor
    int posState = digitalRead(pos);
    if(posChange) {
      lcd.setCursor(0,2);
      lcd.print("          ");
      lcd.setCursor(0,2);
      lcd.print("Pos: ");
      lcd.print(posState);
      digitalWrite(rgbGreen, posState);
    }
  }
  else if(programState == 1) {
    if(animWait < millis()) {
      animWait = millis() + animDelay;

      String orig = "PIR: "+String(pirState)+"  |  Encoder pos: "+String(encoderPos)+"  |  Shock detect: "+String(posState);
      String newText = "                   " + orig + " "; 
      rowText = newText.substring(0+animShift,19+animShift);

      lcd.print("                    ");
      lcd.setCursor(0,0);
      lcd.print(rowText);

      animShift++;
      if(animShift > newText.length()) {
        animShift = 0;
      }
    }
    else if(encoderChange || posChange || pirChange) {
      String orig = "PIR: "+String(pirState)+"  |  Encoder pos: "+String(encoderPos)+"  |  Shock detect: "+String(posState);
      String newText = "                   " + orig + " "; 
      rowText = newText.substring(0+animShift,19+animShift);

      lcd.print("                    ");
      lcd.setCursor(0,0);
      lcd.print(rowText);
    }
  }


  //Check buttons and interrupts
  checkInter();
}