#include<CheapStepper.h>

CheapStepper stepper (8,9,10,11); 
bool forward=true;
int potValue = 0;
int motorSpeed = 0;
int deadZone = 20; 
void setup() {
  stepper.setRpm(22);
  pinMode(A0,INPUT);
}

void loop() {
  potValue = analogRead(A0);
  motorSpeed = map(potValue, 0, 1023, -255, 255);
  if(motorSpeed > -deadZone && motorSpeed < deadZone){
    motorSpeed = 0;
  }
  else if(motorSpeed>0){
    stepper.stepCW();
  }
  else if(motorSpeed<0){
    forward=false;
    stepper.stepCCW();
  }
  forward=true;
}
