/*
Needed components:
    - Arduino Uno       (x1)
    - Raspberry Pi      (x1)
    - USB power bank    (x1)
    - 1.5V AA           (x8)
    - 1.5V AA battery shell (holding 4) (x2)
    - step motor        (x4)
    - RC car framework  (x1)
    - L298N (H-bridge)  (x1)
    - InfraRed obstacle sensor  (x2)
Connections:
    - Pi.USB    <-> Uno.USB
*/
#include "protocol.h"
#include "PiPin.h"
#include "Motor.h"

//#define ENABLE_DEBUG
#include "debug.h"

const char NEW_LINE = 0;
const int SERIAL_BAUDRATE = 19200;

/* Uno <-> Pi GPIO connection:
    PIN_A3   <-> Pi_37   (GPIO_26)   ## bit 0
    PIN_A4   <-> Pi_38   (GPIO_20)   ## bit 1
    PIN_A5   <-> Pi_40   (GPIO_21)   ## bit 2
*/
PiPin pipin(3, 4, 5);

/* L298N connection:
    enA -> PIN_D10
    in1 -> PIN_D9
    in2 -> PIN_D8
    enB -> PIN_D5
    in3 -> PIN_D7
    in4 -> PIN_D6
*/
Motor motor(10, 9, 8, 5, 7, 6);

/* InfraRed obstacle sensor (left) connections:
    VCC  <-> 5V
    GND  <-> GND
    Out  <-> PIN_A0

   InfraRed obstacle sensor (left) connections:
    VCC  <-> 5V
    GND  <-> GND
    Out  <-> PIN_A1
*/
const int LEFT_OBS_PIN  = 0;
const int RIGHT_OBS_PIN = 1;

// internal status for checking obstacles
const byte _STATUS_OBS_NONE         = 0x0;
const byte _STATUS_OBS_LEFT         = 0x1;
const byte _STATUS_OBS_RIGHT        = 0x2;
const byte _STATUS_OBS_FRONT        = 0x4;
const unsigned long FRONT_DIST_THRESHOLD = 200;  // try to avoid front crash when obstacle is 200 mm away
const int MANOEUVRE_TIME    = 400;  // ms

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    motor.init();
    pipin.init();
    randomSeed(analogRead(5));
}

void loop() {
    int op_code, motor_status;
    byte obs_status = _STATUS_OBS_NONE;
    op_code = pipin.read();
    switch (op_code) {
        case OP_CODE_MOTOR_FORWARD:
        case OP_CODE_MOTOR_BACKWARD:
        case OP_CODE_MOTOR_STOP:
        case OP_CODE_MOTOR_SPEEDUP:
        case OP_CODE_MOTOR_SPEEDDOWN:
        case OP_CODE_MOTOR_TURNLEFT:
        case OP_CODE_MOTOR_TURNRIGHT: motor.execute(op_code); break;
        case OP_CODE_ECHO: dbg_print("ECHO"); break;
        default: dbg_print(Response(STATUS_UNSUPPORTED, "").to_s());
    }
    // manoeuvre to avoid front crash
    motor_status = motor.status;
    obs_status |= !(analogRead(LEFT_OBS_PIN) > 512) ? true: false;
    obs_status |= (!(analogRead(RIGHT_OBS_PIN) > 512) ? true: false) << 1;
    obs_status |= ((obs_status&0x1) | ((obs_status>>1)&0x1)) << 2;
    dbg_print(obs_status);
    switch (obs_status) {
        case _STATUS_OBS_LEFT | _STATUS_OBS_RIGHT | _STATUS_OBS_FRONT:
            switch (motor_status) {
                case MOTOR_STATUS_FORWARD:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.stop();
                    if (random(0, 2) == 0) {
                        motor.turnleft(); delay(MANOEUVRE_TIME*2); motor.stop();
                    } else {
                        motor.turnright(); delay(MANOEUVRE_TIME*2); motor.stop();
                    }
                    motor.forward(); break;
                case MOTOR_STATUS_LEFT:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.turnleft();
                    break;
                case MOTOR_STATUS_RIGHT:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.turnright();
                    break;
                case MOTOR_STATUS_BACKWARD: break;
                case MOTOR_STATUS_STOP: break;
                default: break;
            }
            break;
        case _STATUS_OBS_LEFT | _STATUS_OBS_RIGHT:
            switch (motor_status) {
                case MOTOR_STATUS_FORWARD: break;
                case MOTOR_STATUS_LEFT:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.turnleft();
                    break;
                case MOTOR_STATUS_RIGHT:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.turnright();
                    break;
                case MOTOR_STATUS_BACKWARD: break;
                case MOTOR_STATUS_STOP: break;
                default: break;
            }
            break;
        case _STATUS_OBS_LEFT | _STATUS_OBS_FRONT:
            switch (motor_status) {
                case MOTOR_STATUS_FORWARD:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.stop();
                    motor.turnright(); delay(MANOEUVRE_TIME*2); motor.stop();
                    motor.forward(); break;
                case MOTOR_STATUS_LEFT:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.turnleft();
                    break;
                case MOTOR_STATUS_RIGHT:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.turnright();
                    break;
                case MOTOR_STATUS_BACKWARD: break;
                case MOTOR_STATUS_STOP: break;
                default: break;
            }
            break;
        case _STATUS_OBS_RIGHT | _STATUS_OBS_FRONT:
            switch (motor_status) {
                case MOTOR_STATUS_FORWARD:
                    motor.backward(); delay(MANOEUVRE_TIME); motor.stop();
                    motor.turnleft(); delay(MANOEUVRE_TIME*2); motor.stop();
                    motor.forward(); break;
                case MOTOR_STATUS_LEFT: break;
                case MOTOR_STATUS_RIGHT: break;
                case MOTOR_STATUS_BACKWARD: break;
                case MOTOR_STATUS_STOP: break;
                default: break;
            }
            break;
        case _STATUS_OBS_LEFT: break;
        case _STATUS_OBS_RIGHT: break;
        case _STATUS_OBS_FRONT: break;
        default: break;
    }
}
