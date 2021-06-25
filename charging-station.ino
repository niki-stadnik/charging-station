//#include "Keyboard.h"
#include "HID-Project.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

//sliders
unsigned int IntervalSliders = 50;
unsigned long PreviousUpdateSliders = 0;
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A10};
int analogSliderValues[NUM_SLIDERS];

//buttons
unsigned int Cooldown = 300;
const int NUM_BUTTONS = 5;
unsigned long PreviousUpdateButtons[NUM_BUTTONS];
const int buttonPin[NUM_BUTTONS] = {9, 8, 7, 6, 5};
boolean buttonFlag[NUM_BUTTONS];
int buttonState[NUM_BUTTONS];

//charger
unsigned int IntervalCharger = 200; //how often to check status
unsigned long PreviousUpdateCharger = 0;
const int buttonPinRelay = 16;     // the number of the pushbutton pin
int buttonStateRelay = 0;         // variable for reading the pushbutton status
int RelayPin = 15;
boolean chargeFlag = false;
boolean stopFlag = false;
Adafruit_INA219 ina219;
  //float voltage_V = 0,shuntVoltage_mV,busVoltage_V;
float current_mA = 0;

///////////////////////////////////////////////////////////

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    pinMode(buttonPin[i], INPUT_PULLUP);
    buttonFlag[i] = false;
    PreviousUpdateButtons[i] = 0;
  }
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinRelay, INPUT);
  // Set RelayPin as an output pin
  pinMode(RelayPin, OUTPUT);
  
  Serial.begin(9600);
  Consumer.begin();
  Keyboard.begin();
  
  uint32_t currentFrequency;
  ina219.begin();
}

///////////////////////////////////////////////////////////

void loop() {
  //sliders
  if(millis() >= (PreviousUpdateSliders + IntervalSliders)){
    updateSliderValues();
    sendSliderValues();
    PreviousUpdateSliders = millis();
  }

  
  //Charger
  if(millis() >= (PreviousUpdateCharger + IntervalCharger)){
    buttonStateRelay = digitalRead(buttonPinRelay);
    if (buttonStateRelay == HIGH && chargeFlag == false) {      //when headphones on and no chargeing cicle started
      digitalWrite(RelayPin, LOW);                                //start chargeing and start chargeing cicle
      chargeFlag = true;
      delay(1000);
      current_mA = ina219.getCurrent_mA();
    } else if(buttonStateRelay == HIGH && current_mA < 100){    //when headphones on and current low (charged)
      current_mA = ina219.getCurrent_mA();
      if(buttonStateRelay == HIGH && current_mA < 100 && stopFlag == true){
        digitalWrite(RelayPin, HIGH);                               //stop chargeing
        stopFlag = false;
      }
      stopFlag = true;
    } else if(buttonStateRelay == LOW) {                        //when headphones off
      digitalWrite(RelayPin, HIGH);                               //stop chargeing and end chargeing cicle
      chargeFlag = false;
    } else{
      current_mA = ina219.getCurrent_mA();
      stopFlag = false;
    }
    //Serial.println(current_mA);
    //Serial.println(buttonStateRelay);
    //Serial.println(chargeFlag);
    PreviousUpdateCharger = millis();
  }



  //buttons
  /*
  //button 1
  buttonState[0] = digitalRead(buttonPin[0]);
  if (buttonState[0] == LOW) {
    if (buttonFlag[0] == false && millis() >= (PreviousUpdateButtons[0] + Cooldown)){
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_F11);
      buttonFlag[0] = true; 
      Keyboard.releaseAll();
      updateSliderValues();
      sendSliderValues();
      PreviousUpdateButtons[0] = millis();
    }
  } else {
    buttonFlag[0] = false;
  }
  //button 2
  buttonState[1] = digitalRead(buttonPin[1]);
  if (buttonState[1] == LOW) {
    if (buttonFlag[1] == false && millis() >= (PreviousUpdateButtons[1] + Cooldown)){
      Consumer.write(MEDIA_VOL_MUTE);
      buttonFlag[1] = true;
      PreviousUpdateButtons[1] = millis(); 
    }
  } else {
    buttonFlag[1] = false;
  }
*/

//test
  for(int i = 0; i < NUM_BUTTONS; i++){
    buttonState[i] = digitalRead(buttonPin[i]);
    if (buttonState[i] == LOW) {
      if (buttonFlag[i] == false && millis() >= (PreviousUpdateButtons[i] + Cooldown)){
        Buttons(i);
        buttonFlag[i] = true;
        PreviousUpdateButtons[i] = millis(); 
      }
    } else {
      buttonFlag[i] = false;
    }
  }




  

  delay(50);   //it worked good with 10
}

//test

void Buttons(int num) {
  switch(num){
    case 0:
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_F11);
      delay(10);
      Keyboard.releaseAll();
      updateSliderValues();
      sendSliderValues();
      break;
    case 1:
      Consumer.write(MEDIA_VOL_MUTE);
      break;
    case 2:
      Consumer.write(MEDIA_PLAY_PAUSE);
      break;
    case 3:
      Consumer.write(MEDIA_PREVIOUS);
      break;
    case 4:
      Consumer.write(MEDIA_NEXT);
      break;
  }
}


///////////////////////////////////////////////////////////


//sliders
/*
void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}*/

void updateSliderValues() {
  analogSliderValues[0] = analogRead(analogInputs[0]);
  analogSliderValues[1] = analogRead(analogInputs[1]);
  analogSliderValues[2] = analogRead(analogInputs[2]);
  analogSliderValues[3] = analogRead(analogInputs[3]);
  analogSliderValues[4] = analogRead(analogInputs[4]);
}

/*
void sendSliderValues() {
  String builtString = String("");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  Serial.println(builtString);
}*/

void sendSliderValues() {
  String builtString = String("");
  builtString += String((int)analogSliderValues[0]);
  builtString += String("|");
  builtString += String((int)analogSliderValues[1]);
  builtString += String("|");
  builtString += String((int)analogSliderValues[2]);
  builtString += String("|");
  builtString += String((int)analogSliderValues[3]);
  builtString += String("|");
  builtString += String((int)analogSliderValues[4]);
  Serial.println(builtString);
}
