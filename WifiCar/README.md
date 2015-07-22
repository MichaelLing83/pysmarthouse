The ambition of this little project is to build a remote control car connected through Wifi network.

# Materials
- Raspberry Pi Model B+ (x1)
- Arduino Nano (x1)
- USB cable for Arduino Nano
- USB wifi dongle for Pi

# Building steps

## Setup Arduino IDE on Pi

## Test serial over USB
1. connect: Pi USB <-> Nano USB
2. upload sketch ```echoSerial.ino``` to Nano
3. start script ```echoSerial.py``` on Pi
4. type lines in cmd on Pi and Nano should echo whatever sent over back to Pi

# Design

## Communication protocol between Pi and Nano over serial port
- only ASCII characters are used
- it uses a simple command and response mechanism
- procedure on Pi
  - Pi starts in IDLE state
  - in IDLE state, Pi can send a COMMAND and enter WAIT state
  - in WAIT state, Pi waits for a RESPONSE, and enter IDLE state if one arrives or WAIT_TIMER expires
- procedure on Nano
  - Nano starts in IDLE state
  - in IDLE state, Nano waits for one COMMAND and enters WORK state if one arrives
  - in WORK state, Nano executes received COMMAND and sends one RESPONSE after it is done
  - in WORK state, Nano enters IDLE state after a RESPOMSE is sent
- protocol data unit format:
  - each field is represented as <field name>
  - all fields are strings
  - ```"<type>:<type specific fields>;\n"```
  - ```<type>``` can be one of values ```("CMD", "RSP", "DEB")```
- COMMAND:
  - ```"CMD:<operation>[:<paremeter0>:<paremeter1>...:<paremeterN>];\n"```
  - ```<operation>``` can be one of values:
    - ```"ECHO"```: ask Nano to compose a RESPONSE that contains all parsed field of this message
    - ```"TEMPER"```: report temperature using temperature sensor. One paremeter is expected in RESPONSE. One successful RESPONSE could be ```"RSP:OK:27.3;\n```.
    - ```"FORWARD"```: drive motor to move forward
    - ```"BACKWARD"```: drive motor to move backward
    - ```"STOP"```: motor stop
    - ```"SPEEDUP"```: motor speed up
    - ```"SPEEDDOWN"```: motor speed down
    - ```"TURNLEFT"```: motor turn left
    - ```"TURNRIGHT"```: motor turn right
- RESPONSE:
  - ```"RSP:<status>[:<paremeter0>:<paremeter1>...:<paremeterN>];\n"```
  - ```<status>``` can be:
    - ```"OK"```: execution succeeded
    - ```"NOK"```: execution failed
    - ```"NOT_SUPPORTED"```: command is not supported
- DEBUG MESSAGE:
  - ```"DEB:<a string>;```
  - this can only be sent from Nano to Pi, not vice versa.
  - used to send debug message or log. Such a message is not parsed or interpreted by this protocol.
- timers:
  - WAIT_TIMER
    - how much miliseconds should Pi wait after sending a COMMAND before it receives a RESPONSE
    - on expiration: discard current COMMAND, increase COUNTER_WAIT_TIMER, then go to IDLE state
  - RESET_TIMER
    - how much miliseconds should Pi wait after it resets Nano
    - when this timer is bigger than zero, Pi should not send any COMMAND to Nano
- counters:
  - COUNTER_WAIT_TIMER: when it is bigger than MAX_WAIT_TIMER, reset Nano