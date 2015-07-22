/*
    Define a digital pin connection with Raspberry Pi.
*/

#ifndef __PIPIN_H__
#define __PIPIN_H__

#include <Arduino.h>
#include "protocol.h"

const byte PIPIN_OP_CODE_MOTOR_STOP         = 0x0;
const byte PIPIN_OP_CODE_MOTOR_FORWARD      = 0x1;
const byte PIPIN_OP_CODE_MOTOR_BACKWARD     = 0x2;
const byte PIPIN_OP_CODE_MOTOR_TURNLEFT     = 0x3;
const byte PIPIN_OP_CODE_MOTOR_TURNRIGHT    = 0x4;
const byte PIPIN_OP_CODE_MOTOR_SPEEDUP      = 0x5;
const byte PIPIN_OP_CODE_MOTOR_SPEEDDOWN    = 0x6;
const int PIPIN_TO_PROTOCOL_OP_CODES[8] = {OP_CODE_MOTOR_STOP, OP_CODE_MOTOR_FORWARD, OP_CODE_MOTOR_BACKWARD, OP_CODE_MOTOR_TURNLEFT, OP_CODE_MOTOR_TURNRIGHT, OP_CODE_MOTOR_SPEEDUP, OP_CODE_MOTOR_SPEEDDOWN, OP_CODE_UNKNOWN};

class PiPin
{
private:
    int in0, in1, in2;

public:
    PiPin(int _in0, int _in1, int _in2);
    void init(void);
    int read(void);
};

#endif  // __PIPIN_H__
