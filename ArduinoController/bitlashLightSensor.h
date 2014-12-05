/*
    Provide bitlash wrap-up of a light sensor instance.
    Light sensor instance has to be:
        1. named "lightSensor";
        2. exist as global variale
*/

numvar lightSensor_get(void)
{
    return lightSensor.get();
}

numvar lightsensor_pin(void)
{
    return lightSensor.pin();
}

void register_bitlash_lightSensor(void)
{
    addBitlashFunction("lightSensor_get", (bitlash_function) lightSensor_get);
    addBitlashFunction("lightsensor_pin", (bitlash_function) lightsensor_pin);
}