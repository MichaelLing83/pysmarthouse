#!/usr/bin/python3
import serial
import os

if __name__ == '__main__':
    if 'windows' in ' '.join(os.uname()).lower():
        ser = serial.Serial(port=4, baudrate=19200)
    else:
        ser = serial.Serial('/dev/ttyUSB0', 19200)
    while True:
        x = input('--> ')
        ser.write(bytes(x.encode('ascii')))
        while True:
            line = ser.readline()
            decoded = False
            while not decoded:
                try:
                    line = line.decode('ascii')
                    decoded = True
                except UnicodeDecodeError:
                    if len(line) > 0:
                        line = line[1:]
                    else:
                        break
            if decoded:
                print('Nano: ', line)
