/*
    Provide bitlash wrap-up of a Relay instance.
    Relay instance has to be:
        1. named "relay";
        2. exist as global variale
*/

numvar relay_on(void)
{
    relay.on();
    return relay.cur_lvl();
}

numvar relay_off(void)
{
    relay.off();
    return relay.cur_lvl();
}

numvar relay_cur_lvl(void)
{
    return relay.cur_lvl();
}

numvar relay_set_on_lvl(void)
{
    if (getarg(0) == 1)
    {
        relay.on_lvl(getarg(1));
        return relay.cur_lvl();
    }
    return -1;
}

numvar relay_get_on_lvl(void)
{
    return relay.on_lvl();
}

numvar relay_get_pin(void)
{
    return relay.pin();
}