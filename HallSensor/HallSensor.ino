int statusLed = 4 ; // PIN für die LED zur anzeige des Sensor zustandes
int mhSensor = 19 ; // PIN für den Magnetischen Hall Sensors
 
void setup (){
  pinMode (statusLed, OUTPUT) ; // definieren des PIN's für die StatusLED als Ausgangssignal
  pinMode (mhSensor, INPUT) ;  // definieren des PIN's für den Sensor als Eingangssignal
}
 
void loop (){
int val = digitalRead (mhSensor) ; // Lesen des Zustandes des Sensors.
  if (val == LOW){ //Wenn dieser AN ist dann soll die StatusLed leuchten.
    digitalWrite (statusLed, HIGH);
  } else { //Wenn dieser AUS ist dann soll die StatusLed NICHT leuchten.
    digitalWrite (statusLed, LOW);
  }
}
