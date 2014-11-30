/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file UltraSoundDistanceMeas.h
 *
 * Class declaration for UltraSoundDistanceMeas
 */

#ifndef __UltraSoundDistanceMeas_H__
#define __UltraSoundDistanceMeas_H__

#include <Arduino.h>

// speed of ultra sound: 340 m/s = 340 um/us
#define ULTRASOUND_SPEED 340

class UltraSoundDistanceMeas
{
private:
    unsigned int trig_pin;
    unsigned int echo_pin;

public:

    /**
    * @name Primary public interface
    *
    *  These are the main methods you need to operate the chip
    */
    /**@{*/

    /**
    * Constructor
    *
    * Creates a new instance of this driver.  Before using, create
    * an instance and send in the unique pins that this chip is
    * connected to.
    *
    * @param trig_pin The pin attached to Trig
    * @param echo_pin The pin attached to Echo
    */
    UltraSoundDistanceMeas(unsigned int _trig_pin, unsigned int _echo_pin);

    /**
    * Turn light on with given color.
    */
    unsigned long measure(void);
    
    void init(void);

    /**@}*/

};

#endif // __UltraSoundDistanceMeas_H__


