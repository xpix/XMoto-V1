#include <PID_v1.h>

/* Robot_SimpleMotor_Drive_V0 with encoders
 * 
 * Adafruit Feather M4 using Pololu TB6612FNG motor controller
 * Drives two motors at fixed speed with PI control
 * 
 * Motor Control Table
 * XIN1   XIN2    Effect
 * Low    Low     Brake
 * Low    High    Forward
 * High   Low     Reverse
 * 
 * Free to use for all
 * F Milburn, January 2020
 */
// Output pins used to control motors
const uint16_t AIN2 = 12;         // Motor A input 2         Brown
const uint16_t AIN1 = 13;         // Motor A input 1         Green
// Motor encoder external interrupt pins
const uint16_t ENCA = 19;        // Encoder A input         Yellow
// PWM
const uint16_t ANALOG_WRITE_BITS = 8;
const uint16_t MAX_PWM = pow(2, ANALOG_WRITE_BITS)-1;
const uint16_t MIN_PWM = MAX_PWM / 4;    // Make sure motor turns

// Motor timing
unsigned long nowTime = 0;       // updated on every loop
unsigned long startTimeA = 0;    // start timing A interrupts
unsigned long countIntA = 0;     // count the A interrupts
double periodA = 0;              // motor A period
// PID 
const unsigned long SAMPLE_TIME = 100;  // time between PID updates
const unsigned long INT_COUNT = 20;     // sufficient interrupts for accurate timing
double setpointA = 150;         // setpoint is rotational speed in Hz
double inputA = 0;              // input is PWM to motors
double outputA = 0;             // output is rotational speed in Hz
double KpA = 0.20, KiA = 0.20, KdA = 0;
PID motorA(&inputA, &outputA, &setpointA, KpA, KiA, KdA, DIRECT);
double storeB = 0;               // used for debug print

void forwardA(uint16_t pwm){
  digitalWrite(AIN1, pwm);
  digitalWrite(AIN2, LOW);
}
void reverseA(uint16_t pwm){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, pwm);
}
void brakeA(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
}

void initMotors(){
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
}
void initEncoders(){
  pinMode(ENCA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENCA), isr_A, RISING);
}
void initPWM(){
  startTimeA = millis();
  motorA.SetOutputLimits(MIN_PWM, MAX_PWM);
  motorA.SetSampleTime(SAMPLE_TIME);
  motorA.SetMode(AUTOMATIC);
}
void isr_A(){
  // count sufficient interrupts to get accurate timing
  // inputX is the encoder frequency in Hz
  countIntA++;
  if (countIntA == INT_COUNT){
    inputA = (float) INT_COUNT * 1000 / (float)(nowTime - startTimeA);
    startTimeA = nowTime;
    countIntA = 0;
  }
}

void setup(){
 initMotors();
 initEncoders();
 initPWM();
 Serial.begin(9600);
}
void loop(){
  nowTime = millis();
  motorA.Compute();
  forwardA((int)outputA);
  
  if (storeB != outputA){
    storeB = outputA;
    Serial.println("inputA, errorA");
    Serial.print(inputA); Serial.print("  ");
    Serial.print(100*(setpointA-inputA)/setpointA);
    Serial.println("\n");
  }
}
