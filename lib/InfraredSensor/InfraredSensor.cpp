/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "InfraredSensor.h"

InfraredSensor::InfraredSensor(unsigned int __pin): _pin(__pin)
{
}

void InfraredSensor::begin()
{
    pinMode(_pin, INPUT);
}

unsigned int InfraredSensor::pin()
{
    return _pin;
}

bool InfraredSensor::get()
{
    if (digitalRead(_pin) == InfraredSensor_ON)
    {
        return InfraredSensor_ON;
    } else {
        return InfraredSensor_OFF;
    }
}
