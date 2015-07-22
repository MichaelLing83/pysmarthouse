/*
*/
#include "Motor.h"

/****************************************************************************/

Motor::Motor(int _enA, int _in1, int _in2, int _enB, int _in3, int _in4):
    enA(_enA), in1(_in1), in2(_in2), enB(_enB), in3(_in3), in4(_in4)
{
    status                  = MOTOR_STATUS_STOP;
    long_execution_timer    = MOTOR_LONG_EXECUTION_TIMER;
    short_execution_timer   = MOTOR_SHORT_EXECUTION_TIMER;
    speed                   = MOTOR_LOWEST_SPEED;
}

void Motor::init() {
    last_update = millis();
}

void Motor::idle() {
    unsigned long now = millis();
    long_execution_timer -= now - last_update;
    if (long_execution_timer < 0) {
        long_execution_timer = 0;
    }
    short_execution_timer -= now - last_update;
    if (short_execution_timer < 0) {
        short_execution_timer = 0;
    }
    if (long_execution_timer == 0) {
        stop();
        status = MOTOR_STATUS_STOP;
    } else if (short_execution_timer == 0) {
        if (status == MOTOR_STATUS_LEFT || status == MOTOR_STATUS_RIGHT) {
            forward();
            status = MOTOR_STATUS_FORWARD;
        }
    }
}

void Motor::execute(int op_code) {
    switch (op_code) {
        case OP_CODE_MOTOR_FORWARD:   forward(); break;
        case OP_CODE_MOTOR_BACKWARD:  backward(); break;
        case OP_CODE_MOTOR_STOP:      stop(); break;
        case OP_CODE_MOTOR_SPEEDUP:   speedup(); break;
        case OP_CODE_MOTOR_SPEEDDOWN: speeddown(); break;
        case OP_CODE_MOTOR_TURNLEFT:  turnleft(); break;
        case OP_CODE_MOTOR_TURNRIGHT: turnright(); break;
        default: ;
    }
}

void Motor::forward() {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(enA, speed);
    analogWrite(enB, speed);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
}

void Motor::backward() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, speed);
    analogWrite(enB, speed);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
}

void Motor::turnleft() {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, speed);
    analogWrite(enB, speed);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
    short_execution_timer = MOTOR_SHORT_EXECUTION_TIMER;
}

void Motor::turnright() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    analogWrite(enA, speed);
    analogWrite(enB, speed);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
    short_execution_timer = MOTOR_SHORT_EXECUTION_TIMER;
}

void Motor::stop() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
    analogWrite(enA, 0);
    analogWrite(enB, 0);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
}

void Motor::speedup() {
    speed += MOTOR_SPEED_STEP;
    if (speed > MOTOR_HIGHEST_SPEED) {
        speed = MOTOR_HIGHEST_SPEED;
    }
    analogWrite(enA, speed);
    analogWrite(enB, speed);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
    short_execution_timer = MOTOR_SHORT_EXECUTION_TIMER;
}

void Motor::speeddown() {
    speed -= MOTOR_SPEED_STEP;
    if (speed < MOTOR_LOWEST_SPEED) {
        speed = MOTOR_LOWEST_SPEED;
    }
    analogWrite(enA, speed);
    analogWrite(enB, speed);
    long_execution_timer = MOTOR_LONG_EXECUTION_TIMER;
    short_execution_timer = MOTOR_SHORT_EXECUTION_TIMER;
}
