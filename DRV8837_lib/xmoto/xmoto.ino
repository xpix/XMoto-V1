#include "drv8837.h"
#include <Wire.h>

String sdata="";  // Initialised to nothing.

drv8837 motor(12, 13, 19);
int timetorevolution = 3000; // time for one revolution

void setup() {
   //I2C-adress: Slave 5 
   Wire.begin(5);

   //Handler to receive commands
   Wire.onReceive(receiveEvent);  // put your setup code here, to run once:

   // Serial interface for debugging
   Serial.begin(9600);
   Serial.println("Command Interpreter");

   Serial.println("c       - get counter");
   Serial.println("p[0-254]- set speed");
   Serial.println("d[0|1]  - set direction");
   Serial.println("t x     - rotate for X ms");
   Serial.println("s x     - roate x steps");
   Serial.println("r x     - roatate degrees");
   Serial.println("w x     - set time to revolution in ms");


   motor.setSpeed(255); // set to full speed
   motor.setTTR(timetorevolution); // set TTR
}

void loop() {

   if (Serial.available()) {
      byte ch = Serial.read();

      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
         sdata.trim();
         callCommand();
      } // if \r
   }  // available
}

// interprete first char in string as command 
// and rest as integer parameter
void callCommand(){
  int val;

  // Process command in sdata.
  switch( sdata.charAt(0) ) {
   case 'c':
      Serial.print("Counter ");
      Serial.println(motor.count());
      break;
   case 't':
      Serial.println("Set time");
      val = getValue(sdata);
      motor.time(val);
      Serial.print("Val: ");
      Serial.println(val);
      break;
   case 'p':
      Serial.println("Set speed");
      val = getValue(sdata);
      motor.setSpeed(val);
      Serial.print("Val: ");
      Serial.println(val);
      break;
   case 'd':
      Serial.println("Set direction");
      val = getValue(sdata);
      motor.setDirection(val);
      (val ? Serial.println("BACKWARD") : Serial.println("FORWARD"));
      break;
   case 's':
      val = getValue(sdata);
      motor.steps(val);
      Serial.print("Steps Val ");
      Serial.println(val);
      break;
   case 'r':
      val = getValue(sdata);
      motor.deg(val);
      Serial.print("Degrees Val ");
      Serial.println(val);
      break;
   case 'w':
      Serial.println("Set time to rev");
      val = getValue(sdata);
      motor.setTTR(val);
      Serial.print("TTR Val ");
      Serial.println(val);
      break;
   default: Serial.println(sdata);
  } // switch
  
  sdata = ""; // Clear the string ready for the next command.
}

// callback for I2C receive bytes
void receiveEvent(int howMany){
  while(Wire.available())
  {
    byte ch = Wire.read();
    sdata += (char)ch;
  }
  callCommand();
}



int getValue(String sdata){
  int val = 0;
  if (sdata.length()>1){
     String valStr = sdata.substring(1);
     val = valStr.toInt();
  }
  return val;
}
