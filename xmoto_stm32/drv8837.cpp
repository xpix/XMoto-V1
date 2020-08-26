#include "drv8837.h"

/*
 * Die Magnetscheibe lösst 6x für eine Umdrehung aus
 * die Gearbox hat eine Übersetzung von 298:1
 * https://www.pololu.com/product/2208
 * eine Umdrehung hat (6x298) 1788 Impulse
 */

uint8_t _PWM, _AIN1_pin, _AIN2_pin, _LED_pin; // pwm pins
int _stepsToRev, _stepsToMM; //pwm value, ttr
volatile int _counter; // counter for hall sensor
bool _direction, _braked;
long int _steps;
unsigned long _TIMETOSTOP;

// Constructor that sets pins for board
// and use hall sensor pin
drv8837::drv8837(uint8_t AIN1, uint8_t AIN2, uint8_t LED) {
   _AIN1_pin = AIN1;
   _AIN2_pin = AIN2;
   _LED_pin  = LED;
   pinMode(_AIN1_pin, OUTPUT);
   pinMode(_AIN2_pin, OUTPUT);
   pinMode(_LED_pin, OUTPUT);
   _PWM = 0;
   _counter = 0;
}

void drv8837::tick() {
  // check for non blocking delay
  if(_TIMETOSTOP && _TIMETOSTOP < millis()){
    stop();
  }
}

void drv8837::inc_count() {
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
  // set direction
  if(zeit < 0){
    drv8837::setDirection(BACKWARD);
  }else{
    drv8837::setDirection(FORWARD);
  }

  // rotate for specific time
  if( _direction == FORWARD ){
    _forward();
  } else {
    _backward();
  }
  zeit = fabsf(zeit);
  _TIMETOSTOP = millis() + zeit;
}

//rotate motor in degrees
void drv8837::deg(int degrees) {
  float runsteps = ((_stepsToRev / 360.0) * degrees);
  drv8837::steps(round(runsteps));
}

//move motor in millimeter
void drv8837::mov(float mm) {
  if(mm < 0){
    drv8837::setDirection(BACKWARD);
  }else{
    drv8837::setDirection(FORWARD);
  }
  mm = fabsf(mm);
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

void drv8837::stop() {
   digitalWrite(_LED_pin, LOW); // LED set on or off
   analogWrite(_AIN1_pin, 255);
   analogWrite(_AIN2_pin, 255);
   delay(100); // TODO non blocking delay
   _counter = 0;
   _TIMETOSTOP = 0;
}

void drv8837::_forward() {
   digitalWrite(_LED_pin, HIGH); // LED set on or off
   analogWrite(_AIN1_pin, _PWM);
   analogWrite(_AIN2_pin, 1);
}

void drv8837::_backward() {
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
