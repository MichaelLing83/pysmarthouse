/*
*/

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <Arduino.h>

const char SEPARATOR = ':';
const char END_MARK = ';';

const String TYPE_COMMAND = "CMD";
const String TYPE_RESPONSE = "RSP";

// op codes for commands
const int OP_CODE_UNKNOWN         = 0x00;
const int OP_CODE_TEMPERATURE     = 0x01;
const int OP_CODE_ECHO            = 0x02;
const int OP_CODE_MOTOR_FORWARD   = 0x03;
const int OP_CODE_MOTOR_BACKWARD  = 0x04;
const int OP_CODE_MOTOR_STOP      = 0x05;
const int OP_CODE_MOTOR_SPEEDUP   = 0x06;
const int OP_CODE_MOTOR_SPEEDDOWN = 0x07;
const int OP_CODE_MOTOR_TURNLEFT  = 0x08;
const int OP_CODE_MOTOR_TURNRIGHT = 0x09;

// status for responses
const int STATUS_OK             = 0xfe;
const int STATUS_NOK            = 0xfd;
const int STATUS_UNSUPPORTED    = 0xfc;

void send(String a_str);

class Command
{
private:
    String _str;

public:
    int op_code;
    boolean is_ok;
    String op_str;
    String error;
    Command(String _str = "");
    String echo(void);
};

class Response
{
private:
    String msg;
    int status;

public:
    Response(int _status, String _msg);
    String to_s(void);
};

class DebugMsg
{
private:
    String _str;

public:
    DebugMsg(String _str);
    String to_s(void);
};

#endif  // __PROTOCOL_H__
