/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * @file RbgLight.h
 *
 * Class declaration for RbgLight and helper enums
 */

#ifndef __RBGLIGHT_H__
#define __RBGLIGHT_H__

#include <Arduino.h>

#define RBGLIGHT_RED 1
#define RBGLIGHT_BLUE 2
#define RBGLIGHT_GREEN 4

#define RBGLIGHT_PATTERN_PROGRESSING 0

class RbgLight
{
private:
    unsigned int r_pin;
    unsigned int b_pin;
    unsigned int g_pin;
    bool light_on_lvl;
    bool light_off_lvl;
    unsigned int pattern;
    bool pattern_progressing_increse;
    unsigned int pattern_progressing_step;
    unsigned int cur_lvl;

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
    * @param r_pin The pin attached to red controlling pin
    * @param b_pin The pin attached to blue controlling pin
    * @param g_pin The pin attached to green controlling pin
    * @param light_on_lvl HIGH|LOW to trigger LED light on
    */
    RbgLight(unsigned int _r_pin, unsigned int _b_pin,
            unsigned int _g_pin, bool _light_on_lvl);

    void begin(void);

    /**
    * Turn light on with given color.
    */
    void on(unsigned int color);

    /**
    * Turn light off.
    */
    void off(void);
    
    void set_pattern(unsigned int _pattern);
    void flash_pattern(unsigned int color);

    /**@}*/

};

#endif // __RBGLIGHT_H__


