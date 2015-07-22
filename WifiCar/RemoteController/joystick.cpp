/*
 * Joystick library for "Xinda" joysticks.
 * Basically two potentiometers for direction, and a switch for click.
 *
 * This module is based on http://www.arduino.cc/en/Tutorial/JoyStick
 *
 * Available at: https://github.com/axic/arduino-xindajoy
 *
 * Copyright (C) Alex Beregszaszi
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.

 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */

#include "Arduino.h"
#include "XindaJoy.h"

XindaJoy::XindaJoy(int pinX, int pinY, int pinSW)
{
  _pinX = pinX;
  _pinY = pinY;
  _pinSW = pinSW;
}

static int scaleAnalog(int data) {
  return (data * 9 / 1024);
}

void XindaJoy::poll()
{
  _dataX = analogRead(_pinX);
  Serial.println(String("_dataX=") + _dataX);
  // this small pause is needed between reading
  // analog pins, otherwise we get the same value twice
  delay(100);
  _dataY = scaleAnalog(analogRead(_pinY));
  //Serial.println(String("_dataY=") + _dataY);

  if (_pinSW != -1) {
    delay(100);
    _dataSW = analogRead(_pinSW);
  }
}

int XindaJoy::isClicked()
{
  return (_pinSW != -1) && (_dataSW <= 2);
}

int XindaJoy::getDirection()
{
  // FIXME: support 45degree directions also

  // 2 is middle (neutral)
  // <2 is left or down
  // >2 is right or up

  if (_dataX < 2)
    return XINDAJOY_DIR_LEFT;
  else if (_dataX > 2)
    return XINDAJOY_DIR_RIGHT;
  else if (_dataY < 2)
    return XINDAJOY_DIR_DOWN;
  else if (_dataY > 2)
    return XINDAJOY_DIR_UP;
  else
    return XINDAJOY_DIR_NEUTRAL;
}

