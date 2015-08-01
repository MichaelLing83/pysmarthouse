/*
*/

#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <Arduino.h>
#include "protocol.h"

const int MOTOR_STATUS_STOP    = 0x00;
const int MOTOR_STATUS_FORWARD = 0x01;
const int MOTOR_STATUS_LEFT    = 0x02;
const int MOTOR_STATUS_RIGHT   = 0x03;
const int MOTOR_STATUS_BACKWARD= 0x04;

const int MOTOR_LONG_EXECUTION_TIMER    = 5000; // 5 seconds
const int MOTOR_SHORT_EXECUTION_TIMER   = 500;  // 0.5 seconds
const int MOTOR_HIGHEST_SPEED           = 255;
const int MOTOR_LOWEST_SPEED            = 128;
const int MOTOR_SPEED_STEP              = 25;

class Motor
{
private:
    int enA, in1, in2, enB, in3, in4;
    int speed;
    int long_execution_timer;
    int short_execution_timer;
    unsigned long last_update;

public:
    int status;
    Motor(int _enA, int _in1, int _in2, int _enB, int _in3, int _in4);
    void init(void);
    void execute(int op_code);
    void idle(void);
    void forward(void);
    void backward(void);
    void turnleft(void);
    void turnright(void);
    void stop(void);
    void speedup(void);
    void speeddown(void);
};

#endif  // __MOTOR_H__
