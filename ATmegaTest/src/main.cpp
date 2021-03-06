#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCDWIDTH 20
#define LCDHEIGHT 4

//Constants
const int redBtn = 3; //INT 5
const int blueBtn = 2; //INT 4
const int greenBtn = 18; //INT 3
const int blackBtn = 19; //INT 2
const int switch1 = 53; //SWITCH1
const int switch2 = 49; //SWITCH2
const int powerPin = 51; //PP

const int redLeds[3] = {13,12,11};
const int greenLeds[3] = {10,9,8};
const int blueLeds[3] = {7,6,5};
const int yellowLeds[3] = {14,15,16};

LiquidCrystal_I2C lcd(0x27, 20, 4);

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

//LCD Scorller vars, const
int rowIndexes[4] = {0,0,0,0}; //4 Indexy pro 4 radky na displaji 
bool rowScroll[4] = {false,false,false,false}; //Testovani skrolu
int rowScrollSpeed = 200;
unsigned long rowScrollWait = 0;
String rowMsgs[4] = {"","","",""}; 

//Timers
int redLedDelay = 200;
unsigned long redLedWait = 0;

int btnWaitDelay = 200;
unsigned long redBtnWait = 0;
unsigned long blueBtnWait = 0;
unsigned long greenBtnWait = 0;
unsigned long blackBtnWait = 0;

//Functions declaration
void redBtnCallback();
void blueBtnCallback();
void greenBtnCallback();
void blackBtnCallback();
void checkInter();
void repaint();
void scrollClear();
void scrollText();
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

    rowScroll[2] = true;
    rowMsgs[2] = "Ultimatni test displaje xDDD tohle by uz melo pretect";
  }

  //BLUE BUTTON
  if(blueBtnPressed && blueBtnWait < millis()) {
    blueBtnPressed = false;
    blueBtnWait = millis() + btnWaitDelay;
    //CODE

    scrollClear();
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
}

//LCD SCROLLER
void scrollText() {
  for(int i = 0; i < 4; i++) {
    if(rowScroll[i] && rowMsgs[i].length() > 0) {
      lcd.setCursor(0,i);

      if(rowIndexes[i]+20 >= rowMsgs[i].length()) {
        lcd.print(rowMsgs[i].substring(rowIndexes[i],20+rowIndexes[i]));
        rowIndexes[i]--;
      }
      else {
        lcd.print(rowMsgs[i].substring(rowIndexes[i],20+rowIndexes[i]));
        rowIndexes[i]++;
      }
    }
  }
}

//LCD Scroll clearer
void scrollClear() {
  for(int i = 0; i < 4; i++) {
    rowIndexes[i] = 0;
    rowScroll[i] = false;
    lcd.clear();
  }
}

//Repaint display
void repaint() {
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print(lastSwitch1State ? "Switch1 turned ON" : "Switch1 turned OFF");
  lcd.setCursor(0,1);
  lcd.print(lastSwitch2State ? "Switch2 turned ON" : "Switch2 turned OFF");
  lcd.setCursor(0,3);
  lcd.print("Switch sum: ");
  lcd.print(toDec(lastSwitch1State, lastSwitch2State));
}

//Conversi
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
  for(int i = 0; i < 3; i++) { 
    pinMode(redLeds[i], OUTPUT);
    pinMode(blueLeds[i], OUTPUT);
    pinMode(greenLeds[i], OUTPUT);
    pinMode(yellowLeds[i], OUTPUT);
  }

  //Interrupts (Buttons) init
  attachInterrupt(digitalPinToInterrupt(redBtn), redBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(blueBtn), blueBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(greenBtn), greenBtnCallback, RISING);
  attachInterrupt(digitalPinToInterrupt(blackBtn), blackBtnCallback, RISING);

  //LCD init
  lcd.init();
  lcd.backlight();
  displayChanged = true;
}

void loop() {
  //Check switches
  int switch1State = digitalRead(switch1);
  int switch2State = digitalRead(switch2);
  if((switch1State != lastSwitch1State) || (switch2State != lastSwitch2State)) {
    displayChanged = true;
    lastSwitch1State = switch1State;
    lastSwitch2State = switch2State;
  }  

  //Test for change
  if(displayChanged) {
    displayChanged = false;
    repaint();
  }
  checkInter();

  //Text scroller
  if(rowScrollWait < millis()) {
    rowScrollWait = millis() + rowScrollSpeed;
    scrollText();
  }

  //CODE
}