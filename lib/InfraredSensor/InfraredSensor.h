/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
    Connection:
        VCC ->  5V
        GND ->  GND
        OUT  ->  any digital INPUT pin
 */

#ifndef __INFRAREDSENSOR_H__
#define __INFRAREDSENSOR_H__

#define InfraredSensor_ON LOW
#define InfraredSensor_OFF HIGH

#include <Arduino.h>

class InfraredSensor
{
private:
    unsigned int _pin;

public:
    InfraredSensor(unsigned int __pin);
    unsigned int pin(void);
    void begin(void);
    bool get(void);
};

#endif // __INFRAREDSENSOR_H__


