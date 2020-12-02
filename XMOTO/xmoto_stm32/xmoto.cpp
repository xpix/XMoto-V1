#include "xmoto.h"

uint8_t _PWM, _AIN1_pin, _AIN2_pin, _LED_pin; // pwm pins
int _stepsToRev, _stepsToMM; //pwm value, ttr
volatile int _counter; // counter for hall sensor
bool _direction, _braked;
long int _steps;
unsigned long _TIMETOSTOP;

// Constructor that sets pins for board
// and use hall sensor pin
xmoto::xmoto(uint8_t AIN1, uint8_t AIN2, uint8_t LED) {
   _AIN1_pin = AIN1;
   _AIN2_pin = AIN2;
   _LED_pin  = LED;
   pinMode(_AIN1_pin, OUTPUT);
   pinMode(_AIN2_pin, OUTPUT);
   pinMode(_LED_pin, OUTPUT);
   analogWriteFrequency(50);
   _PWM = 0;
   _counter = 0;
   _braked = false;
}

void xmoto::tick() {
  // check for non blocking delay
  if(_TIMETOSTOP && _TIMETOSTOP < millis()){
    stop();
  }
}

void xmoto::inc_count() {
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

long int xmoto::count() {
  return _counter;
}

int xmoto::cerror() {
  return abs(_counter - _steps);
}

//rotate motor for milliseconds
void xmoto::time(int zeit) {
  // set direction
  if(zeit < 0){
    xmoto::setDirection(BACKWARD);
  }else{
    xmoto::setDirection(FORWARD);
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
void xmoto::deg(int degrees) {
  float runsteps = ((_stepsToRev / 360.0) * degrees);
  xmoto::steps(round(runsteps));
}

//move motor in millimeter
void xmoto::mov(float mm) {
  if(mm < 0){
    xmoto::setDirection(BACKWARD);
  }else{
    xmoto::setDirection(FORWARD);
  }
  mm = fabsf(mm);
  float runsteps = (_stepsToMM * mm);
  xmoto::steps(round(runsteps));
}


//rotate motor for x steps
void xmoto::steps(long int steps) {
   _steps = steps;
   if( _direction == FORWARD ){
     _forward();
   } else {
     _backward();
   }
}

void xmoto::stop() {
   analogWrite(_AIN1_pin, 255);
   analogWrite(_AIN2_pin, 255);
   digitalWrite(_LED_pin, LOW); // LED set on or off
   delay(10); // TODO non blocking delay
   _TIMETOSTOP = 0;
}

void xmoto::_forward() {
   _counter = 0;
   analogWrite(_AIN1_pin, _PWM);
   analogWrite(_AIN2_pin, 1);
   _braked = false;
   digitalWrite(_LED_pin, HIGH); // LED set on or off
}

void xmoto::_backward() {
   _counter = 0;
   analogWrite(_AIN1_pin, 1);
   analogWrite(_AIN2_pin, _PWM);
   _braked = false;
   digitalWrite(_LED_pin, HIGH); // LED set on or off
}

bool xmoto::_check_valid() { //check that configuration is set before doing anything
   return (_AIN1_pin && _AIN2_pin);
}

void xmoto::setDirection(bool direction) { //set motor direction
  _direction = direction;
}

void xmoto::setSTR(int steps) { //set ttr
  _stepsToRev = steps;
}

int xmoto::STR() { //set ttr
  return _stepsToRev;
}

void xmoto::setSTM(int steps) { //set ttr
  _stepsToMM = steps;
}

int xmoto::STM() { //set steps/mm
  return _stepsToMM;
}

void xmoto::setSpeed(int speed) { //set motor speed (0-255)
  _PWM = speed;
}

int xmoto::speed() { //get motor speed
      return _PWM;
}

bool xmoto::braked() { //get motor speed
      return _braked;
}
