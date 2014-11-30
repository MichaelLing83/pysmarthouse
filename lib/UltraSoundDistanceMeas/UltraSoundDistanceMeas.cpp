/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "UltraSoundDistanceMeas.h"

/****************************************************************************/
unsigned long UltraSoundDistanceMeas::measure()
{
    unsigned long round_trip_time = 0;
    unsigned long distance = 0;
    digitalWrite(trig_pin, LOW);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(20);
    digitalWrite(trig_pin, LOW);
    round_trip_time = pulseIn(echo_pin, HIGH);
    if (round_trip_time !=0 )
    {
        distance = round_trip_time * (ULTRASOUND_SPEED >> 1 );
    }
    return distance;
}

/****************************************************************************/
UltraSoundDistanceMeas::UltraSoundDistanceMeas(
    unsigned int _trig_pin, unsigned int _echo_pin):
    trig_pin(_trig_pin), echo_pin(_echo_pin)
{
}

void UltraSoundDistanceMeas::init()
{
    pinMode(trig_pin, OUTPUT);
    pinMode(echo_pin, INPUT);
}
