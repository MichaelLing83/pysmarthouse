

const byte numRows= 4; //number of rows on the keypad
const byte numCols= 4; //number of columns on the keypad

//keymap defines the key pressed according to the row and columns just as appears on the keypad
char keymap[numRows][numCols]= 
{
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'}, 
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

/*
  if you hold the keypad with buttons facing you and the cables dropping down, then
  the cables from left to right are:
    rows 0 to 3, and then columns 0 to 3
*/

//Code that shows the the keypad connections to the arduino terminals
byte rowPins[numRows] = {8,7,6,5}; //Rows 0 to 3
byte colPins[numCols] = {12,11,10,9}; //Columns 0 to 3


void setup()
{
  Serial.begin(9600);
}

//If key is pressed, this key is stored in 'keypressed' variable
//If key is not equal to 'NO_KEY', then this key is printed out
//if count=17, then count is reset back to 0 (this means no key is pressed during the whole keypad scan process
const int dpins[8] = {5,6,7,8,9,10,11,12};
const int apins[8] = {26,25,24,23,22,21,20,19};
int avalues[8] = {0};
int dvalues[8] = {0};

void loop()
{
  boolean apressed = false;
  boolean dpressed = false;
  for (int i=0; i<8; i++) {
    avalues[i] = analogRead(apins[i]);
    delay(50);
    dvalues[i] = digitalRead(dpins[i]);
    if (avalues[i] > 500) {
      apressed = true;
    }
    if (dvalues[i] == 1) {
      dpressed = true;
    }
  }
  if (apressed) {
    for (int i=0; i<8; i++) {
      Serial.print(avalues[i]); Serial.print(",");
    }
    Serial.println("");
  }
  if (dpressed) {
    for (int i=0; i<8; i++) {
      Serial.print(dvalues[i]); Serial.print(",");
    }
    Serial.println("");
  }
}

