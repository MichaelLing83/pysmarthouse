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
}
