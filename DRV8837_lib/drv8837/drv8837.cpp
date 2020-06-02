#include "drv8837.h"

/*
 * Die Magnetscheibe lösst 6x für eine Umdrehung aus
 * die Gearbox hat eine Übersetzung von 298:1
 * https://www.pololu.com/product/2208
 * eine Umdrehung hat (6x298) 1788 Impulse
 */

uint8_t _AIN1_pin, _AIN2_pin, _HALL_pin; // pwm pins
int _PWM; //pwm value
volatile long int _counter; // counter for hall sensor
bool _direction;
long int _steps;

// Constructor that sets pins for board - 
// no hall sensor
drv8837::drv8837(uint8_t AIN1, uint8_t AIN2) {
   _AIN1_pin = AIN1;
   _AIN2_pin = AIN2;
   pinMode(_AIN1_pin, OUTPUT);
   pinMode(_AIN2_pin, OUTPUT);
   _PWM = 0;
}

// Constructor that sets pins for board
// and use hall sensor pin
drv8837::drv8837(uint8_t AIN1, uint8_t AIN2, uint8_t HALL) {
   _AIN1_pin = AIN1;
   _AIN2_pin = AIN2;
   _HALL_pin = HALL;
   pinMode(_AIN1_pin, OUTPUT);
   pinMode(_AIN2_pin, OUTPUT);
   pinMode(_HALL_pin, INPUT);
   digitalWrite(_HALL_pin, HIGH);//enable internal pullup resistor
   _PWM = 0;
   _counter = 0;

  attachInterrupt(_HALL_pin, drv8837::_count, RISING);
}

static void drv8837::_count() {
  _counter++;

  if(_steps && _counter >= _steps){
    _stop();
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
  _stop();
}

//rotate motor for x steps
void drv8837::steps(long int steps) {
  // _counter = 0;
  _steps = steps;

  if( _direction == FORWARD ){
    _forward();
  } else {
    _backward();
  }
}

static void drv8837::_stop() {
   analogWrite(_AIN1_pin, 0);
   analogWrite(_AIN2_pin, 0);
}

static void drv8837::_forward() {
   analogWrite(_AIN1_pin, _PWM);
   analogWrite(_AIN2_pin, 1);
}

static void drv8837::_backward() {
   analogWrite(_AIN1_pin, 1);
   analogWrite(_AIN2_pin, _PWM);
}

bool drv8837::_check_valid() { //check that configuration is set before doing anything
   return (_AIN1_pin && _AIN2_pin);
}

void drv8837::setDirection(bool direction) { //set motor direction
  _direction = direction;
}

void drv8837::setSpeed(int speed) { //set motor speed (0-255)
   if (_check_valid())
      _PWM = speed;
}