/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "LightSensor.h"

LightSensor::LightSensor(unsigned int __pin): _pin(__pin)
{
}

void LightSensor::begin()
{
    pinMode(_pin, INPUT);
}

unsigned int LightSensor::pin()
{
    return _pin;
}

bool LightSensor::get()
{
    if (digitalRead(_pin) == LIGHTSENSOR_DARK)
    {
        return LIGHTSENSOR_DARK;
    } else {
        return LIGHTSENSOR_LIGHT;
    }
}
