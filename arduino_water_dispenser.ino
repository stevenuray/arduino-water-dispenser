#include "HX711.h"
HX711 loadcell;

// 1. HX711 circuit wiring
const int LOADCELL_SCK_PIN = 5;
const int LOADCELL_DOUT_PIN = 4;

// 2. Adjustment settings for load cell
const long LOADCELL_DIVIDER = 1214;

// 3. Thresholds
const float BELOW_WEIGHT_THRESHOLD = 40;
const float ABOVE_WEIGHT_THRESHOLD = 180;

// 4. Other config
const unsigned int decimalPlacesToPrintWeight = 2;

const unsigned short enablePin = 8;
const unsigned short inputAPin = 9;
const unsigned short inputBPin = 10;

const unsigned long loopDelayMilliSeconds = 500;
const unsigned long fillDelayMilliseconds = 3000;

// 5. States
enum LiftState {
  BELOW_FILL_WEIGHT_THRESHOLD,
  FILLING,
  FILLING_COOLDOWN,
  ABOVE_FILL_WEIGHT_THRESHOLD
};

bool fillCycleIsStarting = true;

void setup(){
  Serial.begin(9600);
  
  setupLoadCell();
  setupMotorDriver();
}

void loop(){
  // Acquire reading
  float currentWeight = getCurrentWeight(LOADCELL_SCK_PIN);  
  printWeight(currentWeight, decimalPlacesToPrintWeight);
  
  LiftState state = getStateFromWeight(currentWeight);
  
  printState(state);  

  if(state == FILLING && fillCycleIsStarting == true){
    delay(fillDelayMilliseconds);
    fillCycleIsStarting = false;
    return;
  }

  if(state == BELOW_FILL_WEIGHT_THRESHOLD && fillCycleIsStarting == false){
    fillCycleIsStarting = true;
  }

  if(state == ABOVE_FILL_WEIGHT_THRESHOLD && fillCycleIsStarting == false){
    fillCycleIsStarting = true;
  }

  controlValveBasedOnState(state);

  delay(loopDelayMilliSeconds);
}

void controlValveBasedOnState(LiftState state){
  if(state == BELOW_FILL_WEIGHT_THRESHOLD){
    closeValve();
  } else if(state == FILLING){
    openValve();
  } else if(state == ABOVE_FILL_WEIGHT_THRESHOLD){
    closeValve();
  } else {
    Serial.println("Unexpected state detected!");    
  }
}
