#include "drv8837.h"
#include <Wire.h>
#include <PIDController.h>

#define SLAVE 9

String sdata="";  // Initialised to nothing.

PIDController pos_pid; 

drv8837 motor(12, 13, 19);
int stepsToRev = (6 * 298); // time for one revolution 6 steps * 298 (gearbox 1:298)
int oldspeed;


// PID Standard values
int kP = 10;
int kI = 0;
int kD = 2000;

void setup() {
   //I2C-adress: Slave 5 
   Wire.begin(SLAVE);

   //Handler to receive commands
   Wire.onReceive(receiveEvent);  // put your setup code here, to run once:

   // Serial interface for debugging
   Serial.begin(9600);
   Serial.println(" Welcome XMOTO Interface 0.1");

   menu();
   pos_pid.begin();    
   pos_pid.tune(kP, kI, kD);
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
         sdata = ""; // Clear the string ready for the next command.
      } // if \r
   }  // available

   // PID Handling to arrive on specific position
   // you can tune this values above via pid.tune()
   int pos = motor.count();         // get position
   wert = pos_pid.compute(pos);     // compute speed via position
   if(wert > 1){                    // defined value ..
      motor.setSpeed(wert);           // set pwm for speed
   }else{
      pos=0;
      motor.setSpeed(oldspeed);       // set old speed value
   }

   /*
    * TODO: interrupt for breaked, print out if breaked set to true
    */

   
}

// interprete first char in string as command 
// and rest as integer parameter
int callCommand(){
  int val;

  // Process command in sdata.

  // COMMANDS ---------------------------
  switch( sdata.charAt(0) ) {
   case 'c':
      Serial.print("Counter ");
      Serial.println(motor.count());
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
   case 'm':
      val = getValue(sdata);
      motor.mov((float)val);
      Serial.print("move mm ");
      Serial.println(val);
      break;

   // CONFIGURATION ---------------------------
   case 'T':
      Serial.println("Set time");
      val = getValue(sdata);
      motor.time(val);
      Serial.print("Val: ");
      Serial.println(val);
      break;
   case 'P':
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
   case 'D':
      Serial.println("Set direction");
      val = getValue(sdata);
      motor.setDirection(val);
      (val ? Serial.println("BACKWARD") : Serial.println("FORWARD"));
      break;
   case 'W':
      Serial.println("Set/Get steps to rev");
      val = getValue(sdata);
      if(val > 0){
        motor.setSTR(val);
      }
      val = motor.STR();
      Serial.print("STR Val ");
      Serial.println(val);
      break;
   case 'M':
      Serial.println("Set/Get steps to mm");
      val = getValue(sdata);
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
  while(Wire.available())
  {
    byte ch = Wire.read();
    sdata += (char)ch;
  }

  Serial.print("I2C: ");
  Serial.println(sdata);
  
  callCommand();
}

void menu(){
   Serial.println("Command's ----------");
   Serial.println("c       - get counter");
   Serial.println("t x     - rotate for X ms");
   Serial.println("s x     - roate x steps");
   Serial.println("r x     - rotate degrees");
   Serial.println("m x     - move x mm");
   Serial.println("Config --------------");
   Serial.println("P[0-254]- set speed");
   Serial.println("D[0|1]  - set direction");
   Serial.println("W x     - set steps to revolution");
   Serial.println("M x     - set steps to mm");
   Serial.println("K[PID] x- set/get pid tune parameters");
}

void dispPID(){
   Serial.print("PID.tune(kP kI kD) = ");
   Serial.print(kP); Serial.print(",");
   Serial.print(kI); Serial.print(",");
   Serial.print(kD); Serial.println();
}

int getValue(String sdata){
  return getValue(sdata, 1);
}

int getValue(String sdata, int csub  ){
  int val = 0;
  if (sdata.length()>1){
     String valStr = sdata.substring(csub);
     val = valStr.toInt();
  }
  return val;
}
