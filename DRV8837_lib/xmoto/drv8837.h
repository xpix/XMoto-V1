#ifndef DRV8837_H
#define DRV8837_H

#include <Arduino.h>

#define FORWARD 0
#define BACKWARD 1

class drv8837 {
  
  private:
    static void _stop();         //stop motor 
    static void _forward();      //rotate motor forward
    static void _backward();     //rotate motor backward
    bool _check_valid();  //check that configuration is set before doing anything
    static void _count(); //rotate motor forward
    
  public:
    drv8837();                          // default constructor
    drv8837(uint8_t, uint8_t);          // constructor that sets pins for board - AIN1 & AIN2
    drv8837(uint8_t, uint8_t, uint8_t); // constructor that sets pins for board - AIN1/2 and Hall sensor
    void setDirection(bool);            // set motor direction
    void setSpeed(int);                 // set motor speeds
    void setTTR(int);                   // set time to revolution
    void steps(long int);               // rotate motor with x steps
    void deg(int);                  // rotate motor in degrees
    void time(int);                     // rotate motor for milliseconds
    long int drv8837::count();          // get counter value
};

#endif
