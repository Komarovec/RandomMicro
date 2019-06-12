#include <Arduino.h>
#include <Wire.h>
#include "LiquidCrystalScroller.hpp"

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

unsigned long animWait = 0;
int animDelay = 200;

//Rotary encoder vars
int dtLaststate = 0;
int clkLaststate = 0;
int encoderPos = 0;

//PIR vars
int pirLaststate = 0;

//Pos vars
int posLaststate = 0;

//---------------------------------
//---------- ACTIVE CODE ----------
//---------------------------------

//Functions declaration
void redBtnCallback();
void blueBtnCallback();
void greenBtnCallback();
void blackBtnCallback();
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

  redBtnPressed = false;
  blueBtnPressed = false;
  greenBtnPressed = false;
  blackBtnPressed = false;
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
  pinMode(reBtn, INPUT);

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

  //lcd.begin();
  //lcd.scrollRow("Really long text that will definitelly overflow your display :)", 0);
  //lcd.scrollRow("Short txt noscroll", 1);
}

void loop() {
  //lcd.scrollLoop();

  //Rotary encoder logic
  int dtNow = digitalRead(dt);
  int clkNow = digitalRead(clk);
  if(dtNow != dtLaststate) {
    dtLaststate = dtNow;
    if(clkNow == dtNow) {
      encoderPos++;
      cw = true;
      digitalWrite(rgbRed, HIGH);
    }
    else {
      encoderPos--;
      cw = false;
      digitalWrite(rgbRed, LOW);
    }

    lcd.setCursor(12,0);
    lcd.print("       ");
    lcd.setCursor(0,0);
    lcd.print("Encoder pos: "+String(encoderPos));
  }

  //PIR Logic
  int pirState = digitalRead(pir);
  if(pirState != pirLaststate) {
    pirLaststate = pirState;
    lcd.setCursor(0,1);
    lcd.print("          ");
    lcd.setCursor(0,1);
    lcd.print("PIR: ");
    lcd.print(pirState);
    digitalWrite(rgbBlue, pirState);
  }

  //Position sensor
  int posState = digitalRead(pos);
  if(posState != posLaststate) {
    posLaststate = posState;
    lcd.setCursor(0,2);
    lcd.print("          ");
    lcd.setCursor(0,2);
    lcd.print("Pos: ");
    lcd.print(posState);
    digitalWrite(rgbGreen, posState);
  }

  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    lastSwitch1State = switch1State;
    lastSwitch2State = switch2State;
  }  

/*  
  if(animWait < millis()) {
    animWait = millis() + animDelay;
    lcd.setCursor(1,0);
    lcd.print("Animovany text juu toci se jak petr");
  }
*/

  checkInter();

  //CODE
}