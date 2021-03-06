//#include "Keyboard.h"
#include "HID-Project.h"
#include <HID-Settings.h>
#include <Wire.h>
#include <Adafruit_INA219.h>



//buttons
const int NUM_BUTTONS = 5;
const int buttonPin[NUM_BUTTONS] = {9, 8, 7, 6, 5};
boolean buttonState[NUM_BUTTONS];
boolean oldButtonState[NUM_BUTTONS];

//sliders
unsigned int IntervalSliders = 20;
unsigned long PreviousUpdateSliders = 0;
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A10};
int analogSliderValues[NUM_SLIDERS];

//charger
int waitheInterval = 5;                             //how long to waithe for the headphones to start charging in seconds
unsigned int IntervalCharger = 200;                 //how often to check status
int cicles = waitheInterval*1000/IntervalCharger;   //5*1000/200 = 5000/200=25 cicles in 5 seconds
unsigned long PreviousUpdateCharger = 0;
const int buttonPinRelay = 16;                      // the number of the pushbutton pin
int buttonStateRelay = 0;                           // variable for reading the pushbutton status
int RelayPin = 15;
boolean chargeFlag = false;
int stopCounter = 0;
Adafruit_INA219 ina219;
float current_mA = 0;
unsigned long chargedTime = 0;                      //saves the moment the charging started

///////////////////////////////////////////////////////////

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {   //doing both buttons and sliders since they are the same number
    pinMode(analogInputs[i], INPUT);
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
  
  pinMode(buttonPinRelay, INPUT);     // initializing the pushbutton pin as an input
  pinMode(RelayPin, OUTPUT);          // Set RelayPin as an output pin
  
  //Keyboard.begin();
  Consumer.begin();
  Serial.begin(9600);
  
  uint32_t currentFrequency;
  ina219.begin();
}

///////////////////////////////////////////////////////////

void loop() {
  //buttons
  buttonActions(); 

  
  //sliders
  if(millis() >= (PreviousUpdateSliders + IntervalSliders)){
    updateSliderValues();
    sendSliderValues();
    PreviousUpdateSliders = millis();
  }


  //Charger
  /*
  if(millis() >= (PreviousUpdateCharger + IntervalCharger)){
    buttonStateRelay = digitalRead(buttonPinRelay);
    if (buttonStateRelay == HIGH && chargeFlag == false) {      //when headphones on and no charging cicle started
      digitalWrite(RelayPin, LOW);                                //start charging and start charging cicle
      chargeFlag = true;
      current_mA = ina219.getCurrent_mA();
    } else if(buttonStateRelay == HIGH && current_mA < 100){    //when headphones on and current low (charged)
      current_mA = ina219.getCurrent_mA();
      if(stopCounter > cicles){                                 //cicles is calculated based on the time required
        digitalWrite(RelayPin, HIGH);                               //stop charging
        stopCounter = 0;
      }
      stopCounter++;
    } else if(buttonStateRelay == LOW) {                        //when headphones off
      digitalWrite(RelayPin, HIGH);                               //stop charging and end charging cicle
      chargeFlag = false;
      stopCounter = 0;
    } else{
      current_mA = ina219.getCurrent_mA();
      stopCounter = 0;
    }
    //Serial.println(current_mA);
    //Serial.println(buttonStateRelay);
    //Serial.println(chargeFlag);
    //Serial.println(stopCounter);
    PreviousUpdateCharger = millis();
  }*/
  if(millis() >= (PreviousUpdateCharger + IntervalCharger)){
    buttonStateRelay = digitalRead(buttonPinRelay);
    current_mA = ina219.getCurrent_mA();
    if (buttonStateRelay == HIGH && chargeFlag == true){  //headphones on and charging cicle started
      if(current_mA < 39){                               //when headphones on and current low (charged) //35 is too low //40 is too much //37 stops at 85%
        if(stopCounter > cicles){                         //cicles is calculated based on the time required
          digitalWrite(RelayPin, HIGH);                   //stop charging
          stopCounter = 0;
          /*
          if(millis() <= (chargedTime + 15000)){    //that was for auto reset 
            delay(500);
            digitalWrite(RelayPin, LOW);
          }
          */
        }
        stopCounter++;
      }else{
        stopCounter = 0;
      }
    }else if(buttonStateRelay == LOW) {                    //when headphones off
      digitalWrite(RelayPin, HIGH);                        //stop charging and end charging cicle
      chargeFlag = false;
      stopCounter = 0;
    } else{                                               //when headphones on and no charging cicle started
      digitalWrite(RelayPin, LOW);                        //start charging and start charging cicle
      chargeFlag = true;
      current_mA = ina219.getCurrent_mA();
      chargedTime = millis();
    }
    //Serial.println(current_mA);
    //Serial.println(buttonStateRelay);
    //Serial.println(chargeFlag);
    //Serial.println(stopCounter);
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

  }else if(buttonState[0] == HIGH && oldButtonState[0] == LOW){

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
