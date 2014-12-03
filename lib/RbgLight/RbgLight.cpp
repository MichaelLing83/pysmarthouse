/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "RbgLight.h"

/****************************************************************************/
void RbgLight::on(unsigned int color)
{
    if (color & RBGLIGHT_RED)
    {
        digitalWrite(r_pin, light_on_lvl);
    } else {
        digitalWrite(r_pin, light_off_lvl);
    }

    if (color & RBGLIGHT_BLUE)
    {
        digitalWrite(b_pin, light_on_lvl);
    } else {
        digitalWrite(b_pin, light_off_lvl);
    }

    if (color & RBGLIGHT_GREEN)
    {
        digitalWrite(g_pin, light_on_lvl);
    } else {
        digitalWrite(g_pin, light_off_lvl);
    }
}

void RbgLight::begin()
{
    pinMode(r_pin, OUTPUT);
    pinMode(b_pin, OUTPUT);
    pinMode(g_pin, OUTPUT);
    cur_lvl = 10;
}

void RbgLight::flash(unsigned int color, unsigned int interval)
{
    on(color);
    delay(interval);
    off();
}

/****************************************************************************/
void RbgLight::off()
{
    digitalWrite(r_pin, light_off_lvl);
    digitalWrite(b_pin, light_off_lvl);
    digitalWrite(g_pin, light_off_lvl);
}

/****************************************************************************/
RbgLight::RbgLight( unsigned int _r_pin, unsigned int _b_pin,
                    unsigned int _g_pin, bool _light_on_lvl):
    r_pin(_r_pin), b_pin(_b_pin), g_pin(_g_pin), light_on_lvl(_light_on_lvl)
{
    light_off_lvl = !light_on_lvl;
    pattern_progressing_increse = true;
    pattern_progressing_step = 20;
    _default_color = RBGLIGHT_RED;
}

void RbgLight::set_pattern(unsigned int _pattern)
{
    switch (_pattern)
    {
        case RBGLIGHT_PATTERN_PROGRESSING:
            pattern = RBGLIGHT_PATTERN_PROGRESSING;
            break;
        default:
            pattern = RBGLIGHT_PATTERN_PROGRESSING;
    }
}

unsigned int RbgLight::default_color()
{
    return _default_color;
}

void RbgLight::flash_pattern(unsigned int color)
{
    if (color & RBGLIGHT_RED)
    {
        analogWrite(r_pin, cur_lvl);
    } else {
        digitalWrite(r_pin, light_off_lvl);
    }

    if (color & RBGLIGHT_BLUE)
    {
        analogWrite(b_pin, cur_lvl);
    } else {
        digitalWrite(b_pin, light_off_lvl);
    }

    if (color & RBGLIGHT_GREEN)
    {
        analogWrite(g_pin, cur_lvl);
    } else {
        digitalWrite(g_pin, light_off_lvl);
    }
    // calculate next level
    if (pattern_progressing_increse)
    {
        cur_lvl += pattern_progressing_step;
        if (cur_lvl >= 256)
        {
            pattern_progressing_increse = false;
            cur_lvl = 255;
        }
    } else {
        cur_lvl -= pattern_progressing_step;
        if (cur_lvl < 0)
        {
            pattern_progressing_increse = true;
            cur_lvl = pattern_progressing_step;
        }
    }
}
