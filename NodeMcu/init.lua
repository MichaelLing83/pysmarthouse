--init.lua

ds18b20_pin = 3  -- GPIO0
ow.setup(ds18b20_pin)    -- init one wire
cur_temp = -999 -- current temperature from DS18B20

function bxor(a,b)
    local r = 0
    for i = 0, 31 do
        if ( a % 2 + b % 2 == 1 ) then
            r = r + 2^i
        end
        a = a / 2
        b = b / 2
    end
    return r
end

--- Get temperature from DS18B20
function getTemp()
    addr = ow.reset_search(ds18b20_pin)
    repeat
        tmr.wdclr() -- clear system watchdog counter to avoid hardware reset caused by watchdog
        if (addr ~= nil) then
            crc = ow.crc8(string.sub(addr,1,7))
            if (crc == addr:byte(8)) then
                if ((addr:byte(1) == 0x10) or (addr:byte(1) == 0x28)) then
                    ow.reset(ds18b20_pin)
                    ow.select(ds18b20_pin, addr)
                    ow.write(ds18b20_pin, 0x44, 1)
                    tmr.delay(1000000)
                    present = ow.reset(ds18b20_pin)
                    ow.select(ds18b20_pin, addr)
                    ow.write(ds18b20_pin,0xBE, 1)
                    data = nil
                    data = string.char(ow.read(ds18b20_pin))
                    for i = 1, 8 do
                        data = data .. string.char(ow.read(ds18b20_pin))
                    end
                    crc = ow.crc8(string.sub(data,1,8))
                    if (crc == data:byte(9)) then
                        t = (data:byte(1) + data:byte(2) * 256)
                        if (t > 32768) then
                            t = (bxor(t, 0xffff)) + 1
                            t = (-1) * t
                        end
                        t = t * 625
                        cur_temp = t
                        --print("Last temp: " .. cur_temp)
                    end
                    -- tmr.wdclr()
                end
            end
        end
        addr = ow.search(ds18b20_pin)
    until(addr == nil)
end

tmr.alarm(1, 5000, 1, function()
    getTemp()
    t1 = cur_temp / 10000
    t2 = (cur_temp >= 0 and cur_temp % 10000) or (10000 - cur_temp % 10000)
    print("Temp:"..t1 .. "."..string.format("%04d", t2).." C\n")
end)