/*
    Provide bitlash wrap-up of a DS18B20 instance.
    DS18B20 instance has to be:
        1. named "ds18b20";
        2. exist as global variale
*/

// Note that bitlash requires return value to be of type "numvar", which is equivalent to "signed long" in Arduino.
// So the temperature returned here is in unit of 0.001 Degree Celsius.
numvar ds18b20_get_temp(void)
{
    //returns the temperature from one DS18S20 in DEG Celsius

    byte data[12];
    byte addr[8];

    if ( !ds18b20.search(addr)) {
        //no more sensors on chain, reset search
        ds18b20.reset_search();
        return -1000;
    }

    if ( OneWire::crc8( addr, 7) != addr[7]) {
        /* Serial.println("CRC is not valid!"); */
        return -1000;
    }

    if ( addr[0] != 0x10 && addr[0] != 0x28) {
        /* Serial.print("Device is not recognized"); */
        return -1000;
    }

    ds18b20.reset();
    ds18b20.select(addr);
    ds18b20.write(0x44,1); // start conversion, with parasite power on at the end

    byte present = ds18b20.reset();
    ds18b20.select(addr);    
    ds18b20.write(0xBE); // Read Scratchpad


    for (int i = 0; i < 9; i++) { // we need 9 bytes
        data[i] = ds18b20.read();
    }

    ds18b20.reset_search();

    byte MSB = data[1];
    byte LSB = data[0];

    float tempRead = ((MSB << 8) | LSB); //using two's compliment
    float TemperatureSum = tempRead / 16;

    return (numvar)(TemperatureSum * 100);
}

void register_bitlash_ds18b20(void)
{
    addBitlashFunction("ds18b20_get_temp", (bitlash_function) ds18b20_get_temp);
}