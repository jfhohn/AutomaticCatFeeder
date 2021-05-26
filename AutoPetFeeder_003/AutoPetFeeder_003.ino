// define pins for stepper motor 1
#define A 2
#define B 3
#define C 4
#define D 5

// define pins for stepper motor 2
#define E 8
#define F 9
#define G 10
#define H 11
 
#define NUMBER_OF_STEPS_PER_REV 512

#include "Wire.h"
#define DS1307_I2C_ADDRESS 0x68

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
  return( (val/16*10) + (val%16) );
}

//define meal times 4 meal times
int mealOneHour = 9;
int mealOneMinute = 0;
int mealTwoHour = 11;
int mealTwoMinute = 0;
int mealThreeHour = 13;
int mealThreeMinute = 0;
int mealFourHour = 15;
int mealFourMinute = 0;
int mealFiveHour = 17;
int mealFiveMinute = 0;
int mealSixHour = 19;
int mealSixMinute = 0;
int mealSevenHour = 21;
int mealSevenMinute = 0;
int mealEightHour = 23;
int mealEightMinute = 0;

////testing times
//int mealOneHour = 18;
//int mealOneMinute = 5;
//int mealTwoHour = 18;
//int mealTwoMinute = 5;

// define cat portion sizes
//1/4 cup = 100 pellets ~25 rotations
float portionCatBlack = 0.5*100/.25; // black 5/8 cup 55 g
float portionCatGrey = 0.625*100/.25; // grey 1/2 cup 45 g units of pellets

#define catOne 1
#define catTwo 2

void setup(){
//RTC setting time only do once
Wire.begin();
Serial.begin(9600);

//set pins for motor one  
pinMode(A,OUTPUT);
pinMode(B,OUTPUT);
pinMode(C,OUTPUT);
pinMode(D,OUTPUT);

//set pins for motor two
pinMode(E,OUTPUT);
pinMode(F,OUTPUT);
pinMode(G,OUTPUT);
pinMode(H,OUTPUT);

  // set the initial time here:
  // DS3231 seconds, minutes, hours, day, date, month, year
  // setDS1307time(30,50,17,4,12,5,21);
}

void write(int motorNum, int a,int b,int c,int d){
  if (motorNum == catOne){
    digitalWrite(A,a);
    digitalWrite(B,b);
    digitalWrite(C,c);
    digitalWrite(D,d);
  }
  else if (motorNum == catTwo){
    digitalWrite(E,a);
    digitalWrite(F,b);
    digitalWrite(G,c);
    digitalWrite(H,d);
  }
}

void onestep(int motorNum){
int delayTime = 5; // millisecond
//write(motorNum,1,0,0,0);
//delay(delayTime);
write(motorNum,1,1,0,0);
delay(delayTime);
//write(motorNum,0,1,0,0);
//delay(delayTime);
write(motorNum,0,1,1,0);
delay(delayTime);
//write(motorNum,0,0,1,0);
//delay(delayTime);
write(motorNum,0,0,1,1);
delay(delayTime);
//write(motorNum,0,0,0,1);
//delay(delayTime);
write(motorNum,1,0,0,1);
delay(delayTime);
}

/*
 * Can't determine this without testing
 * don't yet know revolutions of motor:screw revolution
 * #motor revolutions = portion size(cups) * 1 revolution/#cups *#motor revolutions/1 revolution
 */
int portionToRevolutions(float portionSize){
  // adjust these parameters with testing
  float pelletsPerScrewRev = 4; //need to measure manually
  int motorRevsPerScrewRev = 2; //need to measure manually
  int motorRevs;
  int feedingSplit = 4;

  motorRevs = int(portionSize / pelletsPerScrewRev * motorRevsPerScrewRev)/feedingSplit;
  Serial.print("Going to perform this many motor revolutions: ");
  Serial.println(motorRevs);
  
  return motorRevs;
}


void feed(int catNum){
  float portionSize; //pellets
  int numRevs;
  
  //feed diff size portion to each cat
  if (catNum == catOne){
    portionSize = portionCatBlack;
  }
  else{
    portionSize = portionCatGrey;
  }
  numRevs = portionToRevolutions(portionSize); // calculate number of revolutions to get correct portion

  //perform numRevs to deposit food
  int i;
  for(i = 0; i < numRevs; i++){
    // perform one revolution
    for (int j = 0; j < NUMBER_OF_STEPS_PER_REV; j++){
      onestep(catNum);
    }
  }  
}

//RTC Functions
void setDS1307time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void readDS1307time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year){
  Wire.beginTransmission(DS1307_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS1307_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
void displayTime(){
  // retrieve data from DS3231
  readDS1307time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  // send it to the serial monitor
//  if (hour == 17){
//    Serial.println("it's 5 o'clock somewhere");
//  }
//  Serial.print(hour, DEC);
//  // convert the byte variable to a decimal number when displayed
//  Serial.print(":");
//  if (minute<10){
//    Serial.print("0");
//  }
//  Serial.print(minute, DEC);
//  Serial.print(":");
//  if (second<10){
//    Serial.print("0");
//  }
//  Serial.print(second, DEC);
//  Serial.print(" ");
//  Serial.print(dayOfMonth, DEC);
//  Serial.print("/");
//  Serial.print(month, DEC);
//  Serial.print("/");
//  Serial.print(year, DEC);
//  Serial.print(" Day of week: ");
//  switch(dayOfWeek){
//  case 1:
//    Serial.println("Sunday");
//    break;
//  case 2:
//    Serial.println("Monday");
//    break;
//  case 3:
//    Serial.println("Tuesday");
//    break;
//  case 4:
//    Serial.println("Wednesday");
//    break;
//  case 5:
//    Serial.println("Thursday");
//    break;
//  case 6:
//    Serial.println("Friday");
//    break;
//  case 7:
//    Serial.println("Saturday");
//    break;
//  }
}

bool canFeed = true; // keep track if pets have been fed recently
int lastTimeFed = 0; // keep track of last time cats fed to update canFeed
void loop(){
  //get time at start of every loop

  displayTime(); // display the real-time clock data on the Serial Monitor,
//  Serial.print("The hour is ");
//  Serial.println(hour);
//  Serial.print("The minute is ");
//  Serial.println(minute);
  Serial.println("Is this loop working?");
  Serial.print("hour is ");
  Serial.println(hour);
  Serial.print("minute is ");
  Serial.println(minute);
  Serial.print("mealOneHour is ");
  Serial.println(mealOneHour);
  //check if it is feeding time for each cat separately
  if ((hour == mealOneHour && minute == mealOneMinute) || (hour == mealTwoHour && minute == mealTwoMinute) ||
       (hour == mealThreeHour && minute == mealThreeMinute) || (hour == mealFourHour && minute == mealFourMinute) ||
       (hour == mealFiveHour && minute == mealFiveMinute) || (hour == mealSixHour && minute == mealSixMinute) ||
       (hour == mealSevenHour && minute == mealSevenMinute) || (hour == mealEightHour && minute == mealEightMinute)) {
    //check cats haven't been fed recently
    Serial.println("Time to eat!");
    if (canFeed){
      //deposit portion of food
      feed(catOne);
      feed(catTwo);// UNCOMMENT WHEN SECOND MOTOR IS ATTACHED
      Serial.println("Cats are fed. Yum!");
      //update boolean to not overfeed cats
      canFeed = false;
//      lastTimeFed = hour;
      lastTimeFed = minute; //UPDATE WHEN RUNNING REAL CODE
      Serial.println("Wait for next feeding!");
    }
  }

  // check if cats have been fed recently and update accordingly
  if (minute > lastTimeFed){ //change back to HOUR later
    canFeed = true;
    Serial.println("Cat can feed again");
  }
  delay(1000);
}
 
