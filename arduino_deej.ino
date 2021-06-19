//#include "Keyboard.h"
#include "HID-Project.h"
#include <Wire.h>
#include <Adafruit_INA219.h>

//sliders
const int NUM_SLIDERS = 5;
const int analogInputs[NUM_SLIDERS] = {A0, A1, A2, A3, A10};
int analogSliderValues[NUM_SLIDERS];

//buttons
const int NUM_BUTTONS = 5;
const int buttonPin[NUM_BUTTONS] = {9, 8, 7, 6, 5};
boolean buttonFlag[NUM_BUTTONS];
int buttonState[NUM_BUTTONS];

//sensor and reley
const int buttonPinRelay = 16;     // the number of the pushbutton pin
int buttonStateRelay = 0;         // variable for reading the pushbutton status
int RelayPin = 15;
boolean chargeFlag = false;
Adafruit_INA219 ina219;
  float voltage_V = 0,shuntVoltage_mV,busVoltage_V;
  float current_mA = 0;
  float power_mW = 0;
  float energy_Wh=0;
  long time_s=0;

///////////////////////////////////////////////////////////

void setup() { 
  for (int i = 0; i < NUM_SLIDERS; i++) {
    pinMode(analogInputs[i], INPUT);
    pinMode(buttonPin[i], INPUT_PULLUP);
    buttonFlag[i] = false;
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
  updateSliderValues();
  sendSliderValues();

  
  //reley and sensor
  //flag will indicate if charge cycle is started
  buttonStateRelay = digitalRead(buttonPinRelay);
  if (buttonStateRelay == HIGH && chargeFlag == false) {
    digitalWrite(RelayPin, LOW);
    chargeFlag = true;
    delay(1000);
    current_mA = ina219.getCurrent_mA();
  } else if(buttonStateRelay == HIGH && current_mA < 100){
    digitalWrite(RelayPin, HIGH);
  } else if(buttonStateRelay == LOW) {
    digitalWrite(RelayPin, HIGH);
    chargeFlag = false;
  } else{
    current_mA = ina219.getCurrent_mA();
  }


  //buttons
  //button 1
  buttonState[0] = digitalRead(buttonPin[0]);
  if (buttonState[0] == LOW) {
    if (buttonFlag[0] == false){
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press(KEY_LEFT_CTRL);
      Keyboard.press(KEY_F11);
      buttonFlag[0] = true; 
      delay(50); Keyboard.releaseAll();
      updateSliderValues();
      sendSliderValues();
    }
  } else {
    buttonFlag[0] = false;
  }
  //button 2
  buttonState[1] = digitalRead(buttonPin[1]);
  if (buttonState[1] == LOW) {
    if (buttonFlag[1] == false){
      Consumer.write(MEDIA_VOL_MUTE);
      buttonFlag[1] = true; 
      delay(50);
    }
  } else {
    buttonFlag[1] = false;
  }

  
  delay(50);   //it worked good with 10
}


///////////////////////////////////////////////////////////


//sliders
void updateSliderValues() {
  for (int i = 0; i < NUM_SLIDERS; i++) {
     analogSliderValues[i] = analogRead(analogInputs[i]);
  }
}
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
