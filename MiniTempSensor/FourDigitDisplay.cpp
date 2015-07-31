/*
*/
#include "FourDigitDisplay.h"

/****************************************************************************/

FourDigitDisplay::FourDigitDisplay(   int _d1, int _d2, int _d3, int _d4,
                        int _sA, int _sB, int _sC, int _sD, int _sE, int _sF, int _sG,
                        int _dp):
    d1(_d1), d2(_d2), d3(_d3), d4(_d4),
    sA(_sA), sB(_sB), sC(_sC), sD(_sD), sE(_sE), sF(_sF), sG(_sG),
    dp(_dp)
{
    int i;

    // set digital selection pin array
    d_pins[0] = d1; d_pins[1] = d2; d_pins[2] = d3; d_pins[3] = d4;
    min_value = -999;
    max_value = 9999;

    // set segment selection pin array
    s_pins[0] = sA; s_pins[1] = sB; s_pins[2] = sC; s_pins[3] = sD;
    s_pins[4] = sE; s_pins[5] = sF; s_pins[6] = sG;
}

void FourDigitDisplay::init() {
    int i;
    for (i=0; i<4; i++) {
        pinMode(d_pins[i], OUTPUT);
        //digitalWrite(d_pins[i], HIGH);
    }
    for (i=0; i<7; i++) {
        pinMode(s_pins[i], OUTPUT);
        //digitalWrite(s_pins[i], LOW);
    }
    pinMode(dp, OUTPUT);
    //digitalWrite(dp, LOW);
    clear();
}

void FourDigitDisplay::select_digit(byte nr) {
    int i;
    switch (nr) {
        case 1:
        case 2:
        case 3:
        case 4:
            for (i=0; i<4; i++) {
                digitalWrite(d_pins[i], HIGH);
            }
            digitalWrite(d_pins[nr-1], LOW);
    }
}

void FourDigitDisplay::digit(byte d, boolean use_dp = false) {
    int i;
    byte pattern;
    if (d < 11) {
        pattern = DISPLAY_PATTERNS[d];
        for (i=0; i<7; i++) {
            digitalWrite(s_pins[i], (pattern >> i) & 1);
        }
        digitalWrite(dp, (pattern >> 7) & 1);   // decimal point for out of range pattern
    }
    if (use_dp) {
        digitalWrite(dp, HIGH);
    }
    delayMicroseconds(10);
}

void FourDigitDisplay::clear() {
    int i;
    for (i=0; i<4; i++) {
        digitalWrite(d_pins[i], HIGH);
    }
    for (i=0; i<7; i++) {
        digitalWrite(s_pins[i], LOW);
    }
    digitalWrite(dp, LOW);
}

void FourDigitDisplay::_prepare(int value) {
    int d_index;
    if (value < min_value || value > max_value) {
        for (d_index=0; d_index<4; d_index++) {
            d_values[d_index] = DISPLAY_PATTERN_OUT_OF_RANGE;
        }
        return;
    }
    boolean is_minus = (value < 0);
    for (d_index=0; d_index<4; d_index++) {
        d_values[d_index] = 255;
    }
    d_index = 3;
    value = abs(value);
    do {
        d_values[d_index--] = value % 10;
        value /= 10;
    } while (value != 0);
    if (is_minus) {
        d_values[d_index] = 10; // minus sign
    }
}

void FourDigitDisplay::_prepare(double value) {
    int d_index;
    if (value < min_value || value > max_value) {
        for (d_index=0; d_index<4; d_index++) {
            d_values[d_index] = DISPLAY_PATTERN_OUT_OF_RANGE;
        }
        return;
    }
    boolean is_minus = (value < 0);
    for (d_index=0; d_index<4; d_index++) {
        d_values[d_index] = 255;
    }
    d_index = 3;
    long d_value = long(abs(value));
    do {
        d_values[d_index--] = d_value % 10;
        d_value /= 10;
    } while (d_value != 0);
    if (is_minus) {
        d_values[d_index] = 10; // minus sign
    }

    d_value = long(abs(value) * 10000);
    for (d_index=3; d_index>-1; d_index--) {
        f_values[d_index] = d_value % 10;
        d_value /= 10;
    }
}

void FourDigitDisplay::display(int value) {
    _prepare(value);
    _display(true);
}

void FourDigitDisplay::display(double value) {
    _prepare(value);
    _display(false);
}

void FourDigitDisplay::display(int value, unsigned long ms) {
    _prepare(value);
    unsigned long ms_prev = millis();
    unsigned long ms_now = ms_prev;
    int i;
    do {
        ms -= ms_now - ms_prev;
        for (i=0; i<1000; i++) {
            _display(true);
        }
        ms_now = millis();
    } while (ms > ms_now - ms_prev);
}

void FourDigitDisplay::display(double value, unsigned long ms) {
    _prepare(value);
    unsigned long ms_prev = millis();
    unsigned long ms_now = ms_prev;
    int i;
    do {
        ms -= ms_now - ms_prev;
        for (i=0; i<1000; i++) {
            _display(false);
        }
        ms_now = millis();
    } while (ms > ms_now - ms_prev);
}

/*
    This function assumes the d_values[] contains process digits and minus sign
    for the integer part, f_values[] contains process digits for the float part.
*/
void FourDigitDisplay::_display(boolean only_int) {
    int d_index, i;
    if (only_int) {
        d_index = 4;
        i = 3;
        do {
            clear();
            select_digit(d_index--);
            digit(d_values[i--]);
        } while (d_values[i] != 255);
    } else {
        d_index = 1;
        for (i=0; i<4; i++) {
            if (d_values[i] != 255) {
                clear();
                select_digit(d_index++);
                if (i==3) {
                    digit(d_values[i], true);
                } else {
                    digit(d_values[i]);
                }
            }
        }
        i = 0;
        while (d_index < 5) {
            clear();
            select_digit(d_index++);
            digit(f_values[i++]);
        }
    }
    clear();
}

