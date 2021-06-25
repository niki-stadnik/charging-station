//#include "Keyboard.h"
#include "HID-Project.h"
#include <HID-Settings.h>
#include <Wire.h>
#include <Adafruit_INA219.h>



//sliders
unsigned int IntervalSliders = 20;
unsigned long PreviousUpdateSliders = 0;
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A10};
int analogSliderValues[NUM_SLIDERS];

//buttons
const int NUM_BUTTONS = 5;
const int buttonPin[NUM_BUTTONS] = {9, 8, 7, 6, 5};
boolean buttonState[NUM_BUTTONS];
boolean oldButtonState[NUM_BUTTONS];

//charger
unsigned int IntervalCharger = 200; //how often to check status
unsigned long PreviousUpdateCharger = 0;
const int buttonPinRelay = 16;     // the number of the pushbutton pin
int buttonStateRelay = 0;         // variable for reading the pushbutton status
int RelayPin = 15;
boolean chargeFlag = false;
boolean stopFlag = false;
Adafruit_INA219 ina219;
float current_mA = 0;

///////////////////////////////////////////////////////////

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinRelay, INPUT);
  // Set RelayPin as an output pin
  pinMode(RelayPin, OUTPUT);
  
  Keyboard.begin();
  Consumer.begin();
  Serial.begin(9600);
  
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

  //buttons
  buttonActions(); 

  
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
}

////////////////////////////////////////////////////////////

void buttonActions(){
  for(int i = 0; i < NUM_BUTTONS; i++){
    oldButtonState[i]=buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);
  }
  if(buttonState[0] == LOW && oldButtonState[0] == HIGH){
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_F11);
    Keyboard.releaseAll();
  }else if(buttonState[0] == HIGH && oldButtonState[0] == LOW){
    
    updateSliderValues();
    analogSliderValues[0] -= 50;
    analogSliderValues[1] -= 50;
    analogSliderValues[2] -= 50;
    analogSliderValues[3] -= 50;
    analogSliderValues[4] -= 50;
    sendSliderValues();
  }
  if(buttonState[1] == LOW && oldButtonState[1] == HIGH){
    Consumer.press(MEDIA_VOL_MUTE);
  }else if(buttonState[1] == HIGH && oldButtonState[1] == LOW){
    Consumer.release(MEDIA_VOL_MUTE);
  }
  if(buttonState[2] == LOW && oldButtonState[2] == HIGH){
    Consumer.press(MEDIA_PLAY_PAUSE);
  }else if(buttonState[2] == HIGH && oldButtonState[2] == LOW){
    Consumer.release(MEDIA_PLAY_PAUSE);
  }
  if(buttonState[3] == LOW && oldButtonState[3] == HIGH){
    Consumer.press(MEDIA_PREVIOUS);
  }else if(buttonState[3] == HIGH && oldButtonState[3] == LOW){
    Consumer.release(MEDIA_PREVIOUS);
  }
  if(buttonState[4] == LOW && oldButtonState[4] == HIGH){
    Consumer.press(MEDIA_NEXT);
  }else if(buttonState[4] == HIGH && oldButtonState[4] == LOW){
    Consumer.release(MEDIA_NEXT);
  }
}



///////////////////////////////////////////////////////////


//sliders

void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}
/*
void updateSliderValues() {
  analogSliderValues[0] = analogRead(analogInputs[0]);
  analogSliderValues[1] = analogRead(analogInputs[1]);
  analogSliderValues[2] = analogRead(analogInputs[2]);
  analogSliderValues[3] = analogRead(analogInputs[3]);
  analogSliderValues[4] = analogRead(analogInputs[4]);
}
*/

void sendSliderValues() {
  String builtString = String("");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogSliderValues[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  Serial.println(builtString);
}
/*
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
}*/
