/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
    Connection:
        # DS18B20
        GND ->  GND
        VDD ->  5V
        DQ  ->  4.7 kOhm    ->  5V
 */

#ifndef __TEMPERATURESENSOR_H__
#define __TEMPERATURESENSOR_H__

#include <Arduino.h>
#include <OneWire.h>

float get_temperature(OneWire* ds_p);

#endif // __TEMPERATURESENSOR_H__


