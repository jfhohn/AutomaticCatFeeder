   /*
  ULN2003-4Phase-Stepper-Motor-Driver
  modified on 06 Dec 2020
  by Amir Mohammad Shojaee @ Electropeak
  https://electropeak.com/learn/
  
  based on Arduino library 
  trying new tepper code
*/

#include <Stepper.h>
const int stepsPerRevolution = 2048;
const int rpm = 12;
Stepper stepper1 = Stepper(stepsPerRevolution, 2, 3, 4, 5);

void setup() {
  stepper1.setSpeed(rpm);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  Serial.println("Select rotation: ");
  Serial.println("1.1/4 forward");
  Serial.println("2.1/2 forward");
  Serial.println("3.full forward");
}

void loop() {
  if (Serial.available() > 0) {
    // read the incoming byte:
   int incomingByte = Serial.read();
    
  switch (incomingByte) { 
    case '1':         // if input=1 ....... motors turn One_quarter 
      One_quarter();
      break;
    case '2':         // if input=2 ....... motors turn One_half
      One_half();
      break;
    case '3':         // if input=1 ....... motors turn full
      full();
      break;
  }
  delay(200);
  incomingByte=0;
}
 
}
 
void One_quarter(){
  stepper1.step(stepsPerRevolution/4);
  delay(100);
}
void One_half(){
  stepper1.step(stepsPerRevolution/2);
  delay(100);
}
void full(){
  stepper1.step(stepsPerRevolution);
  delay(100);
}
