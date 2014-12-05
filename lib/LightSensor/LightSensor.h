/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
    Connection:
        VCC ->  5V
        GND ->  GND
        DO  ->  any digital INPUT pin
 */

#ifndef __LIGHTSENSOR_H__
#define __LIGHTSENSOR_H__

#define LIGHTSENSOR_LIGHT LOW
#define LIGHTSENSOR_DARK HIGH

#include <Arduino.h>

class LightSensor
{
private:
    unsigned int _pin;

public:
    LightSensor(unsigned int __pin);
    unsigned int pin(void);
    void begin(void);
    bool get(void);
};

#endif // __LIGHTSENSOR_H__


