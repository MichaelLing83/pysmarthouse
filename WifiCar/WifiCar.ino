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
    PIN_2   <-> Pi_37   (GPIO_26)   ## bit 0
    PIN_3   <-> Pi_38   (GPIO_20)   ## bit 1
    PIN_4   <-> Pi_40   (GPIO_21)   ## bit 2
*/
PiPin pipin(2, 3, 4);

/* L298N connection:
    enA -> PIN_10
    in1 -> PIN_9
    in2 -> PIN_8
    enB -> PIN_5
    in3 -> PIN_7
    in4 -> PIN_6
*/

Motor motor(10, 9, 8, 5, 7, 6);

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
    motor.init();
    pipin.init();
}

void loop() {
    int op_code;
    op_code = pipin.read();
    switch (op_code) {
        case OP_CODE_MOTOR_FORWARD:
        case OP_CODE_MOTOR_BACKWARD:
        case OP_CODE_MOTOR_STOP:
        case OP_CODE_MOTOR_SPEEDUP:
        case OP_CODE_MOTOR_SPEEDDOWN:
        case OP_CODE_MOTOR_TURNLEFT:
        case OP_CODE_MOTOR_TURNRIGHT: motor.execute(op_code); dbg_print(Response(STATUS_OK, "").to_s()); break;
        case OP_CODE_ECHO: dbg_print("ECHO"); break;
        default: dbg_print(Response(STATUS_UNSUPPORTED, "").to_s());
    }
    delay(50);
}
