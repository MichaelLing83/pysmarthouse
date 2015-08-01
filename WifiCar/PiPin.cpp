/*  To save digital pin usage on Arduino, this library assumes analog pins
    are used instead.
*/
#include "PiPin.h"

/****************************************************************************/

PiPin::PiPin(int _in0, int _in1, int _in2):
    in0(_in0), in1(_in1), in2(_in2)
{
}

void PiPin::init() {
    pinMode(in0, INPUT_PULLUP);
    pinMode(in1, INPUT_PULLUP);
    pinMode(in2, INPUT_PULLUP);
}

int PiPin::read() {
    byte first_read, second_read;
    boolean is_jitter = true;
    while (is_jitter) {
        first_read = (analogRead(in0)>512?1:0) | ((analogRead(in1)>512?1:0) << 1) | ((analogRead(in2)>512?1:0) << 2);
        delay(1);
        second_read = (analogRead(in0)>512?1:0) | ((analogRead(in1)>512?1:0) << 1) | ((analogRead(in2)>512?1:0) << 2);
        if (first_read == second_read) {
            is_jitter = false;
        }
    }
    return PIPIN_TO_PROTOCOL_OP_CODES[first_read];
}
