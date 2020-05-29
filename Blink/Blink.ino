/*
  Blink on Attiny1617

  Turns an LED on port 4 for one second, then off for one second, repeatedly.
  Test also the serial/UART Interface

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(4, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("Blink");
  digitalWrite(4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(4, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
