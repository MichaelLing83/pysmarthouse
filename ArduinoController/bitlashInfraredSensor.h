/*
    Provide bitlash wrap-up of a infrared sensor instance.
    Light sensor instance has to be:
        1. named "infraredSensor";
        2. exist as global variale
*/

numvar infraredSensor_get(void)
{
    return infraredSensor.get();
}

numvar infraredSensor_pin(void)
{
    return infraredSensor.pin();
}

void register_bitlash_infraredSensor(void)
{
    addBitlashFunction("infraredSensor_get", (bitlash_function) infraredSensor_get);
    addBitlashFunction("infraredSensor_pin", (bitlash_function) infraredSensor_pin);
}