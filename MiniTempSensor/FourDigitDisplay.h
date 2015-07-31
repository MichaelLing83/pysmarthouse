/*
4-digit 7-segment display model: F2481AH
Hold the display panel towards you, and all the decimal points at the downside, then
the pins from down-left to down-right, and then top-right to top-left are:
    PIN_1 to PIN_7, PIN_8 to PIN_14

Basics:
    1. one digit is composed of 7 segments
    2. one digit is selected if its pin is LOW
    3. one segment of the selected digit is selected if its pin is HIGH

Selections:
    digit_1     <-  PIN_12
    digit_2     <-  PIN_1
    digit_3     <-  PIN_5
    digit_4     <-  PIN_9
    segment_A   <-  PIN_14
    segment_B   <-  PIN_8
    segment_C   <-  PIN_6
    segment_D   <-  PIN_2,7
    segment_E   <-  PIN_4
    segment_F   <-  PIN_13
    segment_G   <-  PIN_10
    decimal_point   <-  PIN_3
    colon       <-  PIN_11  // not used in this lib

    A
    __
  F|__| B   the middle one is G
  E|__| C
    D

Connect all pins to digital pins on Arduino, if analog pin is used, in constructor
use analog pin number + 100. E.g. if digit_1 pin is connected to PIN_A1 on
Arduino, then when calling the constructor give the pin number as 101.
Note that this is only valid for the four digit selection pins.
*/

#ifndef __FOURDIGITDISPLAY_H__
#define __FOURDIGITDISPLAY_H__

#include <Arduino.h>

// display patterns for digits 0 to 9, here 1 means light up. so in program they
// need to be binary negated
// bit 0 is segment_A, bit 6 is segment_G
const byte DISPLAY_PATTERN_ZERO     = B00111111;
const byte DISPLAY_PATTERN_ONE      = B00000110;
const byte DISPLAY_PATTERN_TWO      = B01011011;
const byte DISPLAY_PATTERN_THREE    = B01001111;
const byte DISPLAY_PATTERN_FOUR     = B01100110;
const byte DISPLAY_PATTERN_FIVE     = B01101101;
const byte DISPLAY_PATTERN_SIX      = B01111101;
const byte DISPLAY_PATTERN_SEVEN    = B00000111;
const byte DISPLAY_PATTERN_EIGHT    = B01111111;
const byte DISPLAY_PATTERN_NINE     = B01101111;
const byte DISPLAY_PATTERN_MINUS    = B01000000;    // the minus sign
const byte DISPLAY_PATTERN_OUT_OF_RANGE = B11111111;
const byte DISPLAY_PATTERNS[12] = {DISPLAY_PATTERN_ZERO, DISPLAY_PATTERN_ONE,
    DISPLAY_PATTERN_TWO, DISPLAY_PATTERN_THREE, DISPLAY_PATTERN_FOUR,
    DISPLAY_PATTERN_FIVE, DISPLAY_PATTERN_SIX, DISPLAY_PATTERN_SEVEN,
    DISPLAY_PATTERN_EIGHT, DISPLAY_PATTERN_NINE, DISPLAY_PATTERN_MINUS };

class FourDigitDisplay
{
private:
    int min_value, max_value;   // minimum and maximum value can be displayed
    int d1, d2, d3, d4; // digit selection pins
    int d_pins[4];
    int sA, sB, sC, sD, sE, sF, sG; // segment selection pins
    int s_pins[7];
    int dp; // decimal point selection pin
    byte d_values[4] = {255};
    byte f_values[4] = {0};

public:
    FourDigitDisplay(   int _d1, int _d2, int _d3, int _d4,
                        int _sA, int _sB, int _sC, int _sD, int _sE, int _sF, int _sG,
                        int _dp);
    void init(void);
    void clear(void);
    void select_digit(byte nr); // select one digit
    void digit(byte d, boolean use_dp);   // display one digit
    void _prepare(int value);
    void _prepare(double value);
    void _display(boolean only_int);
    void display(int value);
    void display(int value, unsigned long ms);    // display given value for ms milliseconds.
    void display(double value);
    void display(double value, unsigned long ms);
};

#endif  // __FOURDIGITDISPLAY_H__
