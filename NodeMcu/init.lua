--init.lua

devide_id = "05"
ds18b20_pin = 3  -- GPIO0
pin = 4 -- GPIO1
ow.setup(ds18b20_pin)    -- init one wire
wifi_ssid = "michael"
wifi_passwd = "waterpigs"
config_server_port = 5683
data_server_ip = "192.168.31.185"
data_server_port = 9999
data_report_timer = 60000   -- 60 seconds

-- Bit xor operation
--- TODO: is this not built in??
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

-- Get temperature from DS18B20
--- return: integer, unit is 1/10000 degree celcius; a value over 100 C should be treated as invalid
function getTemp()
    local t = 101 * 10000    -- 101 degree Celcius, the invalid temperature
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
                        -- print("Last temp: " .. t)
                    end
                end
            end
        end
        addr = ow.search(ds18b20_pin)
    until(addr == nil)
    return t
end

wifi.setmode(wifi.STATION)
wifi.sta.config(wifi_ssid, wifi_passwd)
wifi.sta.connect()

tmr.alarm(1, 1000, 1, function()
    if wifi.sta.getip()== nil then
      print("IP unavaiable, Waiting...")
    else
        tmr.stop(1)
        print("Config done, IP is "..wifi.sta.getip())
        print("start UDP server on port "..config_server_port)
        s=net.createServer(net.UDP)
        s:on("receive",function(s,c)
            --print(c)
            if c == "high" then
                print("turning GPIO2 high")
                gpio.write(pin, gpio.HIGH)
            else
                print("turning GPIO2 low")
                gpio.write(pin, gpio.LOW)
            end
        end)
        s:listen(config_server_port)
        client = net.createConnection(net.UDP, 0)
        client:connect(data_server_port, data_server_ip)
        tmr.alarm(2, data_report_timer, 1, function()
            cur_temp = getTemp()
            t1 = cur_temp / 10000
            t2 = (cur_temp >= 0 and cur_temp % 10000) or (10000 - cur_temp % 10000)
            print("Temp:"..t1.."."..string.format("%04d", t2).." C\n")
            client:send("REPORT;"..devide_id..";Temperature;"..cur_temp)
        end)
    end
end)
