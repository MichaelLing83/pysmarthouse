/*
 Copyright (C) 2014 Michael Duo Ling <duo.ling.cn@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

#include "Relay.h"

/****************************************************************************/
Relay::Relay(uint8_t __pin, uint8_t __on_lvl):
    _pin(__pin), _on_lvl(__on_lvl)
{
}

void Relay::begin(uint8_t default_lvl)
{
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, default_lvl);
    _cur_lvl = default_lvl;
}

void Relay::on(void)
{
    digitalWrite(_pin, _on_lvl);
    _cur_lvl = _on_lvl;
}

void Relay::off(void)
{
    digitalWrite(_pin, !_on_lvl);
    _cur_lvl = !_on_lvl;
}

uint8_t Relay::cur_lvl(void)
{
    return _cur_lvl;
}

void Relay::on_lvl(uint8_t __on_lvl)
{
    _on_lvl = __on_lvl;
}
uint8_t Relay::on_lvl(void)
{
    return _on_lvl;
}

uint8_t Relay::pin(void)
{
    return _pin;
}
