/*
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
        first_read = digitalRead(in0) | (digitalRead(in1) << 1) | (digitalRead(in2) << 2);
        delay(1);
        second_read = digitalRead(in0) | (digitalRead(in1) << 1) | (digitalRead(in2) << 2);
        if (first_read == second_read) {
            is_jitter = false;
        }
    }
    return PIPIN_TO_PROTOCOL_OP_CODES[first_read];
}
