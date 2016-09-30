#ifndef __CURRENT_SENSOR_CPP__
#define __CURRENT_SENSOR_CPP__

#include "CurrentSensor.h"

CurrentSensor::CurrentSensor(int _pin):
  pin(_pin){
}

float CurrentSensor::read() {
  return (analogRead(pin) - 512) * CURRENT_RESOLUTION;
}

#endif  // __CURRENT_SENSOR_CPP__
