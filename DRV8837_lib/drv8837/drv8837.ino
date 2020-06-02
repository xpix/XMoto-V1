#include "drv8837.h"

String sdata="";  // Initialised to nothing.

drv8837 motor(12, 13, 19);

void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600);
   Serial.println("Command Interpreter");

   motor.setSpeed(255); // set to full speed
}

void loop() {

   if (Serial.available()) {
      byte ch = Serial.read();

      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
         sdata.trim();
         int val;

         /*  
          motor.time(1000);   // move for 1000ms (1 second)
          delay(2000);
          motor.degree(90);   // move 90 degress
          delay(2000);
         */

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
           default: Serial.println(sdata);
         } // switch

         sdata = ""; // Clear the string ready for the next command.
      } // if \r
   }  // available
  
}

int getValue(String sdata){
  int val = 0;
  if (sdata.length()>1){
     String valStr = sdata.substring(1);
     val = valStr.toInt();
  }
  return val;
}
