#include <Arduino.h>

/* Left joy stick
    VCC <-> 5V, GND <-> GND
    VRx <-> PIN_A0
    VRy <-> PIN_A1
    SW  <-> PIN_D2
   Right joy stick
    VCC <-> 5V, GND <-> GND
    VRx	<-> PIN_A2
    VRy <-> PIN_A3
    SW  <-> PIN_D3
*/

const byte JOYSTICK_NONE  = 0x00;
const byte JOYSTICK_LEFT  = 0x01;
const byte JOYSTICK_RIGHT = 0x02;
const byte JOYSTICK_UP    = 0x04;
const byte JOYSTICK_DOWN  = 0x08;
const byte JOYSTICK_CLICK = 0x10;

const int LEFT_JOYSTICK_VRX_PIN  = 0;
const int LEFT_JOYSTICK_VRY_PIN  = 1;
const int LEFT_JOYSTICK_SW_PIN   = 2;
const int RIGHT_JOYSTICK_VRX_PIN = 2;
const int RIGHT_JOYSTICK_VRY_PIN = 3;
const int RIGHT_JOYSTICK_SW_PIN  = 3;

const int JOYSTICK_MIN      = 0;
const int JOYSTICK_MAX      = 1023;
const int JOYSTICK_MARGIN   = 100;

byte joystick_left_init(int sw_pin = LEFT_JOYSTICK_SW_PIN)
{
    pinMode(sw_pin, INPUT_PULLUP);
}

byte joystick_right_init(int sw_pin = RIGHT_JOYSTICK_SW_PIN)
{
    pinMode(sw_pin, INPUT_PULLUP);
}

byte joystick_left(int vrx_pin = LEFT_JOYSTICK_VRX_PIN,
                   int vry_pin = LEFT_JOYSTICK_VRY_PIN,
                   int sw_pin  = LEFT_JOYSTICK_SW_PIN)
{
    int x, y, sw = 0;
    byte status = JOYSTICK_NONE;
    x = analogRead(vrx_pin);
    y = analogRead(vry_pin);
    sw = digitalRead(sw_pin);
    //Serial.print(String("left x=") + x + "\ty=" + y + "\tsw=" + sw);
    if (x < JOYSTICK_MIN + JOYSTICK_MARGIN) {
        status |= JOYSTICK_DOWN;
    } else if (x > JOYSTICK_MAX - JOYSTICK_MARGIN) {
        status |= JOYSTICK_UP;
    }
    if (y < JOYSTICK_MIN + JOYSTICK_MARGIN) {
        status |= JOYSTICK_RIGHT;
    } else if (y > JOYSTICK_MAX - JOYSTICK_MARGIN) {
        status |= JOYSTICK_LEFT;
    }
    if (sw == 0) {
        status |= JOYSTICK_CLICK;
    }
    //Serial.println(String("\tstatus=") + status);
    return status;
}

byte joystick_right(int vrx_pin = RIGHT_JOYSTICK_VRX_PIN,
                   int vry_pin = RIGHT_JOYSTICK_VRY_PIN,
                   int sw_pin  = RIGHT_JOYSTICK_SW_PIN)
{
    int x, y, sw = 0;
    byte status = JOYSTICK_NONE;
    x = analogRead(vrx_pin);
    y = analogRead(vry_pin);
    sw = digitalRead(sw_pin);
    //Serial.print(String("right x=") + x + "\ty=" + y + "\tsw=" + sw);
    if (x < JOYSTICK_MIN + JOYSTICK_MARGIN) {
        status |= JOYSTICK_LEFT;
    } else if (x > JOYSTICK_MAX - JOYSTICK_MARGIN) {
        status |= JOYSTICK_RIGHT;
    }
    if (y < JOYSTICK_MIN + JOYSTICK_MARGIN) {
        status |= JOYSTICK_UP;
    } else if (y > JOYSTICK_MAX - JOYSTICK_MARGIN) {
        status |= JOYSTICK_DOWN;
    }
    if (sw == 0) {
        status |= JOYSTICK_CLICK;
    }
    //Serial.println(String("\tstatus=") + status);
    return status;
}

void setup()
{
  Serial.begin(19200);
  joystick_left_init();
  joystick_right_init();
}

void loop()
{
  int left_joystick_status, right_joystick_status;
  left_joystick_status = joystick_left();
  right_joystick_status = joystick_right();
  delay(400);
}
