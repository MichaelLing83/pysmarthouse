/*
*/

#ifndef __CMD_H__
#define __CMD_H__

#include <Arduino.h>

class Cmd
{
private:
    String _cmdS;

public:

    // constructor with a cmd string, without the leading "CMD;"
    Cmd(String __cmdS);
    // execute commands
    void execute(void);
};

#endif  // __CMD_H__