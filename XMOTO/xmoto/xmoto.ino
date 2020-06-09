#include "drv8837.h"
#include <Wire.h>
#include <PIDController.h>


String sdata="";  // Initialised to nothing.

PIDController pos_pid; 

drv8837 motor(12, 13, 19);
int stepsToRev = (6 * 298); // time for one revolution 6 steps * 298 (gearbox 1:298)
int oldspeed;

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
   Serial.println("w x     - set steps to revolution in ms");

   pos_pid.begin();    
   pos_pid.tune(10, 0, 2000);    
   pos_pid.limit(-255, 255);
  
   motor.setSpeed(255); // set to full speed
   oldspeed = motor.speed();

   motor.setSTR(stepsToRev); // set STR
}

int wert = 0;

void loop() {

   if (Serial.available()) {
      byte ch = Serial.read();

      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
         sdata.trim();
         callCommand();
      } // if \r
   }  // available

   // PID Handling to arrive on specific position
   // you can tune this values above via pid.tune()
   int pos = motor.count();         // get position
   wert = pos_pid.compute(pos);     // compute speed via position
   if(wert > 1){                    // defined value ..
    motor.setSpeed(wert);           // set pwm for speed
   }else{                           
    motor.setSpeed(oldspeed);       // set old speed value
   }
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
      if(val > 0){
        motor.setSpeed(val);
        oldspeed=motor.speed();
      }
      val = motor.speed();
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
      pos_pid.setpoint(val);
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
      Serial.println("Set/Get steps to rev");
      val = getValue(sdata);
      if(val > 0){
        motor.setSTR(val);
      }
      val = motor.STR();
      Serial.print("STR Val ");
      Serial.println(val);
      break;
   default: Serial.println(sdata);
  } // switch
  
  sdata = ""; // Clear the string ready for the next command.

  // check flag is motor run and braked
  if(motor.braked()){
      Serial.println("Stopped");
  }

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
