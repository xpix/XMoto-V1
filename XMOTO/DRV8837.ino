// Define motor pWM output variable
int PWMout;

// We'll be controlling the motors from pins 3, 4, 5, 6, and 20, 21, 22, and 23.
// This must be one of the PWM-capable pins.
const int AIN1 = 12;
const int AIN2 = 13;

void setup()
{
   //Initialize serial and wait for port to open:
  Serial.begin(9600);

  // define motor control pins as output
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

}

void loop()
{
  Serial.println("Forward");
 
 for(int ii = 130; ii < 256; ii+=30) {  // slowly ramp the motors to full speed forward
   PWMout = ii;
   Serial.println(PWMout);
   analogWrite(AIN1, PWMout);
   analogWrite(AIN2, 0);
   delay(200);
 }

 Serial.println("Backward");

 for(int ii = 130; ii < 256; ii+=30) {  // slowly ramp the motors to full speed reverse
   PWMout = ii;
   Serial.println(PWMout);
   analogWrite(AIN1, 0);
   analogWrite(AIN2, PWMout);
   delay(200);
 }

 // Stall
 Serial.println("Stall");
 analogWrite(AIN1, 0);
 analogWrite(AIN2, 0);
 
 delay(2000);
}
