#ifndef XMOTO_H
#define XMOTO_H

#include <Arduino.h>

#define FORWARD 0
#define BACKWARD 1

class xmoto {
  
  private:
    static void _stop();         //stop motor 
    static void _forward();      //rotate motor forward
    static void _backward();     //rotate motor backward
    bool _check_valid();  //check that configuration is set before doing anything
    
  public:
    xmoto();                          // default constructor
    xmoto(uint8_t, uint8_t, uint8_t); // constructor that sets pins for board - AIN1 & AIN2

    void setDirection(bool);            // set motor direction
    void setSpeed(int);                 // set motor speeds
    int speed();                        // get speed

    void setSTM(int);                   // set steps per mm
    int  STM();                         // get steps per mm

    void setSTR(int);                   // set steps per revolution
    int  STR();                         // get steps per revolution

    void stop();                        // stop motor and clear all data
    void steps(long int);               // rotate motor with x steps
    void deg(int);                      // rotate motor in degrees
    void mov(float);                    // rotate motor in millimeter
    void time(int);                     // rotate motor for milliseconds
    long int count();                   // get counter value
    void inc_count();                   // set counter external

    bool braked();                      // get status of motor after run
    void tick();                        // check for values and enviroment
};

#endif
