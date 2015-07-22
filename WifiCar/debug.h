/*
    Define debug macros.
*/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <Arduino.h>

#ifdef ENABLE_DEBUG
    #define dbg_print(a_str) Serial.println(a_str)
#else
    #define dbg_print(a_str)
#endif

#endif  // __DEBUG_H__
