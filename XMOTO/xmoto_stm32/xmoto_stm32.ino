#include "xmoto.h"
#include <PIDController.h>

// For STM32F04 we can only switch on I2C or Serial, 
// otherwise too much FLASH is used
// Hardwareserial the same, 
// use instead SoftwareSerial with less FLASH consumption
// set serial to 9600 and just CR, commands w/o space i.e. s100

#define UART       // Switch serial ON
//#define I2C        // Switch I2C ON

#define MASTER 8   // I2C Adress from Master 
#define SLAVE  9   // I2C Adress from this xmoto device
#define HALL 9
#define LED 3

#if !defined(STM32_CORE_VERSION) || (STM32_CORE_VERSION  < 0x01090000)
#error "Due to API change, this sketch is compatible with STM32_CORE_VERSION  >= 0x01090000"
#endif

#if defined(I2C)
#include <Wire.h>
#endif

#if defined(UART)
#include <SoftwareSerial.h>
//SoftwareSerial Serial(PA10, PA9); // RX, TX on I2C pins
SoftwareSerial Serial(PA3, PA2); // RX, TX
#endif

String sdata="";     // Initialised to nothing.
String toMASTER="";  // Initialised to nothing.

PIDController pos_pid; 

//xmoto motor(12, 13, 19, 4); // ATTINY pins: AIN1, AIN2, HALL, LED
xmoto motor(A0, A1, LED); // STM32 pins: AIN1, AIN2, HALL, LED
int stepsToRev = (3 * 298); // time for one revolution 3 steps * 298 (gearbox 1:298)
int oldspeed;
volatile unsigned int hcounter = 0;

// PID Standard values
float kP = 5;
float kI = 1;
float kD = 0.01;

void setup() {
   digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
   delay(500);
   digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW

   Serial.begin(9600);
   Serial.println("Welcome");

   menu();
   pos_pid.begin();    
   pos_pid.limit(5, 254);
   pos_pid.tune(kP, kI, kD);
  
   motor.setSpeed(255); // set to full speed
   oldspeed = motor.speed();

   motor.setSTR(stepsToRev); // set STR

   // STM32 Interrupt
   pinMode(HALL, INPUT_PULLUP);
   attachInterrupt(digitalPinToInterrupt(HALL), hcount, FALLING); 
}


uint8_t wert = 0;

void loop() {

   if (Serial.available()) {
      byte ch = Serial.read();

      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
        sdata.trim();
        callCommand();
        sdata = "";
      } // if \r
   }  // available


   // PID Handling to arrive on specific position
   // you can tune this values above via pid.tune()
   int pos = motor.count();         // get position
   wert = pos_pid.compute(pos);     // compute speed via position
   if(wert > 5 && wert < 255){                    // defined value ..
      motor.setSpeed(wert);           // set pwm for speed
   }else{
      pos=0;
      motor.setSpeed(oldspeed);       // set old speed value
   }

   // If motor stopped and toMASTER then send back
#if defined(I2C)
   if(motor.braked() && toMASTER.length() >= 1){
      WireSend(MASTER, toMASTER);
      toMASTER="";
   }
#endif

   // update motor values and check for nonblocking delay
   motor.tick();
}

void hcount() {
   motor.inc_count();
}


// interprete first char in string as command 
// and rest as integer parameter
int callCommand(){
  float val;

  // Process command in sdata.

  // COMMANDS ---------------------------
  switch( sdata.charAt(0) ) {
   case 'c':
      Serial.print("Counter ");
      Serial.println(motor.count());
      break;
   case 'q':
      Serial.print("STOP ");
      motor.stop();
      Serial.println(motor.count());
      break;
   case 't':
      Serial.println("Set time");
      val = getValue(sdata, 1);
      motor.time(val);
      Serial.print(F("Val: "));
      Serial.println(val);
      break;
   case 's':
      val = getValue(sdata, 1);
      pos_pid.setpoint(val);
      motor.steps(val);
      Serial.print(F("Steps: "));
      Serial.println(val);
      break;
   case 'r':
      val = getValue(sdata, 1);
      motor.deg(val);
      Serial.print("Degrees Val ");
      Serial.println(val);
      break;
   case 'm':
      val = getValue(sdata, 1);
      motor.mov(val);
      Serial.print("move mm ");
      Serial.println(val);
      break;

   // CONFIGURATION ---------------------------
   case 'P':
      Serial.println("Set speed");
      val = getValue(sdata, 1);
      if(val > 0){
        motor.setSpeed(val);
        oldspeed=motor.speed();
      }
      val = motor.speed();
      Serial.print("Val: ");
      Serial.println(val);
      break;
   case 'D':
      Serial.println("Set direction");
      val = getValue(sdata, 1);
      motor.setDirection(val);
      (val ? Serial.println("BACKWARD") : Serial.println("FORWARD"));
      break;
   case 'W':
      Serial.println("Set/Get steps to rev");
      val = getValue(sdata, 1);
      if(val > 0){
        motor.setSTR(val);
      }
      val = motor.STR();
      Serial.print("STR Val ");
      Serial.println(val);
      break;
   case 'M':
      Serial.println("Set/Get steps to mm");
      val = getValue(sdata, 1);
      if(val > 0){
        motor.setSTM(val);
      }
      val = motor.STM();
      Serial.print("STM Val ");
      Serial.println(val);
      break;
   case 'K':
      Serial.println("Set/Get tune pid (kP kI kD)");
      switch( sdata.charAt(1) ) {
       case 'P':
          kP = getValue(sdata, 2);
          dispPID();
          break;
       case 'I':
          kI = getValue(sdata, 2);
          dispPID();
          break;
       case 'D':
          kD = getValue(sdata, 2);
          dispPID();
          break;
       default: 
          dispPID();
      } // switch(k)
      pos_pid.tune(kP, kI, kD);
      break;
   default: 
    menu();
  } // switch

  return val;
}

// callback for I2C receive bytes
void receiveEvent(int howMany){
#if defined(I2C)
  while(Wire.available())
  {
    byte ch = Wire.read();
    sdata += (char)ch;
  }

  Serial.print(F("I2C: "));
  Serial.println(sdata);
  
  int val = callCommand();
  toMASTER = String(val, DEC);
  sdata="";
#endif
}

void WireSend(byte adress, String txt) {
#if defined(I2C)
  Serial.print(F("Send via I2C: "));
  Serial.println(txt);
  char buffer[8];
  txt.toCharArray(buffer, 8);
  Wire.beginTransmission(adress);
  Wire.write(buffer);
  Wire.endTransmission();
  delay(50);
#endif
}


void menu(){
   Serial.println(F("Command's ----------"));
   Serial.println(F("c       - get counter"));
   Serial.println(F("t x     - rotate for X ms"));
   Serial.println(F("s x     - roate x steps"));
   Serial.println(F("r x     - rotate degrees"));
   Serial.println(F("m x     - move x mm"));
   Serial.println(F("q x     - stop motor"));
   Serial.println(F("Config --------------"));
   Serial.println(F("P[0-254]- set speed"));
   Serial.println(F("D[0|1]  - set direction"));
   Serial.println(F("W x     - set steps to revolution"));
   Serial.println(F("M x     - set steps to mm"));
   Serial.println(F("K[PID] x- set/get pid tune parameters"));
}

void dispPID(){
   Serial.print(F("PID.tune(kP kI kD) = "));
   Serial.print(kP); Serial.print(F(","));
   Serial.print(kI); Serial.print(F(","));
   Serial.print(kD); Serial.println();
}

float getValue(String sdata){
  return getValue(sdata, sdata.indexOf(' ')+1);
}

float getValue(String sdata, int csub  ){
  float val = 0.000;
  if (sdata.length()>1){
     String valStr = sdata.substring(csub);

     // Replace the value.toFloat method to save a LOT of memory!
     char cbuff[valStr.length()+1];
     valStr.toCharArray(cbuff,valStr.length()+1);//Converts String into character array
     val = stringToFloat(cbuff);
  }
  return val;
}

float stringToFloat(char string[])
{
    float result= 0.0;
    int len = strlen(string);
    int dotPosition = 0;

    for (int i = 0; i < len; i++)
    {
      if (string[i] == '.')
      {
        dotPosition = len - i  - 1;
      }
      else
      {
        result = result * 10.0 + (string[i]-'0');
      }
    }

    while (dotPosition--)
    {
      result /= 10.0;
    }

    return result;
}
