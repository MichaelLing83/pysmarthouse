# Control 3-Pin (RBG) LED lights
  
Design Goals: This library is designed to be...  
  
* Easy control of RBG LED lights
* Support many flashing patterns (only one is implemented now...)
  
Supported Boards:  
  
* Uno

## Pin layout

The table below shows how to connect the the pins of the RBG LED light
to Arduino UNO board.
R, B, and G pins are configurable.

| PIN | RBG LED  | Arduino UNO | ATtiny25/45/85 [0] | ATtiny44/84 [1] |
|-----|----------|-------------|--------------------|-----------------|
|  1  |    V     |     5V      |                    |                 |
|  2  |    R     |      8      |                    |                 |
|  3  |    B     |      9      |                    |                 |
|  4  |    G     |     10      |                    |                 |


**Constructor:**

    RbgLight(unsigned int _r_pin, unsigned int _b_pin,
            unsigned int _g_pin, bool _light_on_lvl);
    _r_pin: which pin is connected to R;
    _b_pin: which pin is connected to B;
    _g_pin: which pin is connected to G;
    _light_on_lvl: activation level (usually HIGH activate light).