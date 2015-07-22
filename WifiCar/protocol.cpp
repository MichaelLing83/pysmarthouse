/*
*/
#include "protocol.h"

/****************************************************************************/
void send(String a_str) {
    Serial.println(a_str);
}

Command::Command(String __str):
    _str(__str), error("")
{
    int first_separator_index, second_separator_index, end_mark_index;
    String temp_str;
    is_ok = true;
//send( DebugMsg("Command::Command _str=" + _str).to_s() );
    if (!_str.startsWith(TYPE_COMMAND)) {
        is_ok = false;
        error = _str + " is not a COMMAND";
    }
    if (is_ok) { // find the first separator
        first_separator_index = _str.indexOf(SEPARATOR);    // it should always be "CMD:xxxxx"
//send( DebugMsg("Command::Command first_separator_index=" + first_separator_index).to_s() );
        if (first_separator_index == -1) {
            // no SEPARATOR found, this is wrong
            is_ok = false;
            error = _str + " is missing separator";
        }
    }
    if (is_ok) { // find the end mark
        end_mark_index = _str.indexOf(END_MARK);
//send( DebugMsg("Command::Command end_mark_index=" + end_mark_index).to_s() );
        if (end_mark_index == -1) {
            // no end mark is found
            is_ok = false;
            error = _str + " is missing end mark";
        }
    }
    if (is_ok && end_mark_index <= first_separator_index) {
        // end mark is before the first separator, wrong
        is_ok = false;
        error = _str + " has end mark before the first separator";
    }
    if (is_ok) { // find the second separator
        second_separator_index = _str.indexOf(SEPARATOR, first_separator_index + 1);
//send( DebugMsg("Command::Command second_separator_index=" + second_separator_index).to_s() );
        if (second_separator_index == -1) {
            // not found, then there is only one separator
            second_separator_index = end_mark_index;
        }
//send( DebugMsg("Command::Command end_mark_index=" + end_mark_index).to_s() );
//send( DebugMsg("Command::Command second_separator_index=" + second_separator_index).to_s() );
        // get <operation> field
        temp_str = _str.substring(first_separator_index+1, second_separator_index);
//send( DebugMsg("Command::Command temp_str=" + temp_str).to_s() );
        if (temp_str.length() == 0) {
            // <operation> field is empty
            is_ok = false;
            error = _str + " has an empty operation field";
        }
    }
//send( DebugMsg("Command::Command error=" + error).to_s() );
    if (is_ok) { // decide op_code
        op_str = temp_str;
//send( DebugMsg("Command::Command op_str=" + op_str).to_s() );
        if (op_str == "TEMPER") {
            op_code = OP_CODE_TEMPERATURE;
        } else if (op_str == "ECHO") {
            op_code = OP_CODE_ECHO;
        } else if (op_str == "FORWARD") {
            op_code = OP_CODE_MOTOR_FORWARD;
        } else if (op_str == "BACKWARD") {
            op_code = OP_CODE_MOTOR_BACKWARD;
        } else if (op_str == "STOP") {
            op_code = OP_CODE_MOTOR_STOP;
        } else if (op_str == "SPEEDUP") {
            op_code = OP_CODE_MOTOR_SPEEDUP;
        } else if (op_str == "SPEEDDOWN") {
            op_code = OP_CODE_MOTOR_SPEEDDOWN;
        } else if (op_str == "TURNLEFT") {
            op_code = OP_CODE_MOTOR_TURNLEFT;
        } else if (op_str == "TURNRIGHT") {
            op_code = OP_CODE_MOTOR_TURNRIGHT;
        } else {
            op_code = OP_CODE_UNKNOWN;
        }
    }
}

String Command::echo() {
    return Response(STATUS_OK, op_str).to_s();
}

Response::Response(int _status, String _msg):
    status(_status), msg(_msg)
{
}

String Response::to_s() {
    String status_str;
    switch (status) {
        case STATUS_OK: status_str = "OK"; break;
        case STATUS_UNSUPPORTED: status_str = "NOT_SUPPORTED"; break;
        default: status_str = String("NOK:");
    }
    return String("RSP:") + status_str + ":" + msg + ";";
}

DebugMsg::DebugMsg(String __str):
    _str(__str)
{
}

String DebugMsg::to_s() {
    return String("DEB:") + _str + ";";
}
