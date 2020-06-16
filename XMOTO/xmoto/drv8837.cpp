#include "drv8837.h"

/*
 * Die Magnetscheibe lösst 6x für eine Umdrehung aus
 * die Gearbox hat eine Übersetzung von 298:1
 * https://www.pololu.com/product/2208
 * eine Umdrehung hat (6x298) 1788 Impulse
 */

uint8_t _AIN1_pin, _AIN2_pin, _HALL_pin, _LED_pin; // pwm pins
int _PWM, _stepsToRev, _stepsToMM; //pwm value, ttr
volatile int _counter; // counter for hall sensor
bool _direction, _braked;
long int _steps;

// Constructor that sets pins for board - 
// no hall sensor
drv8837::drv8837(uint8_t AIN1, uint8_t AIN2, uint8_t LED) {
   _AIN1_pin = AIN1;
   _AIN2_pin = AIN2;
   _LED_pin  = LED;
   pinMode(_AIN1_pin, OUTPUT);
   pinMode(_AIN2_pin, OUTPUT);
   pinMode(_LED_pin, OUTPUT);
   _PWM = 0;
}

// Constructor that sets pins for board
// and use hall sensor pin
drv8837::drv8837(uint8_t AIN1, uint8_t AIN2, uint8_t HALL, uint8_t LED) {
   _AIN1_pin = AIN1;
   _AIN2_pin = AIN2;
   _HALL_pin = HALL;
   _LED_pin  = LED;
   pinMode(_AIN1_pin, OUTPUT);
   pinMode(_AIN2_pin, OUTPUT);
   pinMode(_LED_pin, OUTPUT);
   _PWM = 0;
   _counter = 0;
   pinMode(_HALL_pin, INPUT_PULLUP);
   PORTA.PIN2CTRL|=0x02; //ISC=2 trigger rising - uses |= so current value of 
}

ISR(PORTA_PORT_vect) {
  byte flags=PORTA.INTFLAGS;
  PORTA.INTFLAGS=flags; //clear flags
    _counter++;
    if(_steps && _counter >= _steps){
      analogWrite(_AIN1_pin, 255);
      analogWrite(_AIN2_pin, 255);
      _braked = true;
      digitalWrite(_LED_pin, LOW); // LED set on or off
    }else{
      _braked = false;
    }
}

long int drv8837::count() {
  return _counter;
}

//rotate motor for milliseconds
void drv8837::time(int zeit) {
  if( _direction == FORWARD ){
    _forward();
  } else {
    _backward();
  }
  delay(zeit); // TODO non blocking delay
  stop();
}

//rotate motor in degrees
void drv8837::deg(int degrees) {
  float runsteps = ((_stepsToRev / 360.0) * degrees);
  drv8837::steps(round(runsteps));
}

//move motor in millimeter
void drv8837::mov(float mm) {
  float runsteps = (_stepsToMM * mm);
  drv8837::steps(round(runsteps));
}


//rotate motor for x steps
void drv8837::steps(long int steps) {
   _counter = 0;
   _steps = steps;
   if( _direction == FORWARD ){
     _forward();
   } else {
     _backward();
   }
}

static void drv8837::stop() {
   digitalWrite(_LED_pin, LOW); // LED set on or off
   analogWrite(_AIN1_pin, 255);
   analogWrite(_AIN2_pin, 255);
   delay(100); // TODO non blocking delay
   _counter = 0;
}

static void drv8837::_forward() {
   digitalWrite(_LED_pin, HIGH); // LED set on or off
   analogWrite(_AIN1_pin, _PWM);
   analogWrite(_AIN2_pin, 1);
}

static void drv8837::_backward() {
   digitalWrite(_LED_pin, HIGH); // LED set on or off
   analogWrite(_AIN1_pin, 1);
   analogWrite(_AIN2_pin, _PWM);
}

bool drv8837::_check_valid() { //check that configuration is set before doing anything
   return (_AIN1_pin && _AIN2_pin);
}

void drv8837::setDirection(bool direction) { //set motor direction
  _direction = direction;
}

void drv8837::setSTR(int steps) { //set ttr
  _stepsToRev = steps;
}

int drv8837::STR() { //set ttr
  return _stepsToRev;
}

void drv8837::setSTM(int steps) { //set ttr
  _stepsToMM = steps;
}

int drv8837::STM() { //set steps/mm
  return _stepsToMM;
}

void drv8837::setSpeed(int speed) { //set motor speed (0-255)
   if (_check_valid())
      _PWM = speed;
}

int drv8837::speed() { //get motor speed
      return _PWM;
}

bool drv8837::braked() { //get motor speed
      return _braked;
}

