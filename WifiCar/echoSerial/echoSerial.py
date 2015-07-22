#!/usr/bin/python3
import serial

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyUSB0', 19200)
    while True:
        x = input('--> ')
        ser.write(bytes(x.encode('ascii')))
        print('Nano: ', ser.readline().decode('ascii'))
