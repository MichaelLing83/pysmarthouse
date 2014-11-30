# Control ultra sound distance measuring chip
  
Design Goals: This library is designed to be...  
  
* Easy control ultra sound distance measuring chip
  
Supported Boards:  
  
* Uno

## Pin layout

The table below shows how to connect the the pins of the RBG LED light
to Arduino UNO board.
R, B, and G pins are configurable.

| PIN | RBG LED  | Arduino UNO | ATtiny25/45/85 [0] | ATtiny44/84 [1] |
|-----|----------|-------------|--------------------|-----------------|
|  1  |   VCC    |     5V      |                    |                 |
|  2  |   Trig   |      9      |                    |                 |
|  3  |   Echo   |      8      |                    |                 |
|  4  |   GND    |     GND     |                    |                 |


**Constructor:**

    UltraSoundDistanceMeas(unsigned int _trig_pin, unsigned int _echo_pin);
    _trig_pin: which pin is connected to Trig;
    _echo_pin: which pin is connected to Echo;

**Usage:**

    See example. Note that measured distance is in micrometers.
    
    