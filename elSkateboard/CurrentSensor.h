#ifndef __CURRENT_SENSOR_H__
#define __CURRENT_SENSOR_H__

#include <Arduino.h>
#define CURRENT_RESOLUTION  0.0391  // = 20A / 512

class CurrentSensor
{
  private:
    int pin;

  public:
    CurrentSensor(int _pin);
    float read();
};
#endif  // __CURRENT_SENSOR_H__
