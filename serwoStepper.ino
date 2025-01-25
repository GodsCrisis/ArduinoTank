#include <Servo.h>


Servo myservo;

int left;
int right;
int pos = 90;
void moveRight(){
  int pos2;
  
  for(pos2 = pos;pos2>=90;pos--){
    right = digitalRead(5);
    if(right == 1){
    	break;
    }
    myservo.write(pos);
    delay(15);
  }
  
}

void moveLeft(){
  int pos2;
  
  for(pos2 = pos;pos2<=120;pos++){
    left = digitalRead(6);
    if(left == 1){
    	break;
    }
    myservo.write(pos);
    delay(15);
  }
}

void setup() {
  Serial.begin(9600);
  myservo.attach(9);  
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
}

void loop() {
  left = digitalRead(6);
  right = digitalRead(5);
  Serial.println(pos);
  if(left == 0){
  	moveLeft();
  }
  else if(right == 0){
    moveRight();
  }                                           

}