#define SERIAL_BAUDRATE 19200

void setup(){
    Serial.begin(SERIAL_BAUDRATE);
}

void loop(){
    if (Serial.available()) {
        String s = Serial.readStringUntil(0);   // read until new line character
        Serial.print(s);
    }
}
