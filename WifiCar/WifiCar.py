#!/usr/bin/python3
import logging
import argparse
import os
import socketserver
from time import sleep
from SerialProtocol.protocol import RxCmd, NotRxCmdException
import RPi.GPIO as gpio

"""
class Connection:
    TYPES = (SERIAL, ) = range(1)
    def __init__(self, conn_type=SERIAL):
        self.conn_type = conn_type
        if conn_type == Connection.SERIAL:
            if 'windows' in ' '.join(os.uname()).lower():
                self.conn = serial.Serial(port=4, baudrate=19200)
            else:
                self.conn = serial.Serial('/dev/ttyACM0', 19200)
    def send(self, a_str):
        if self.conn_type == Connection.SERIAL:
            self.conn.write(bytes(a_str.encode('ascii')))
            logging.debug("Pi->Uno: {}".format(a_str))
            self.conn.flush()
    def read_char(self):
        c = self.conn.read()
        decoded = False
        while not decoded:
            try:
                c = c.decode('ascii')
                decoded = True
            except UnicodeDecodeError:
                c = self.conn.read()
        return c
    def receive(self):
        line = self.conn.readline()
        decoded = False
        while not decoded:
            try:
                line = line.decode('ascii')
                decoded = True
            except UnicodeDecodeError:
                line = line[1:]
        logging.debug("Uno->Pi: {}".format(line))
        return line
    def readlines(self):
        temp = ""
        lines = tuple()
        if self.conn_type == Connection.SERIAL:
            while self.conn.inWaiting() > 0:
                temp += self.conn.read(1)
            decoded_str = ""
            for c in temp:
                try:
                    decoded_str += c.decode('ascii')
                except UnicodeDecodeError:
                    pass
            lines = tuple(decoded_str.split('\n'))
        for line in lines:
            logging.debug("Uno->Pi: {}".format(line))
        return lines
"""

class PiPin:
    def __init__(self, out_pins):
        '''
            out_pins: a list of GPIO pins for output command.

            Note that the pin numbering is the board numbering starting from 1.
        '''
        assert isinstance(out_pins, tuple) or isinstance(out_pins. list)
        assert len(out_pins) == 3
        for pin in out_pins:
            assert isinstance(pin, int)
            assert 0 < pin <= 40
        self.out_pins = out_pins
        gpio.setmode(gpio.BOARD)
        for pin in self.out_pins:
            gpio.setup(pin, gpio.OUT)
            gpio.output(pin, False)
    def write(self, value):
        for i in range(len(self.out_pins)):
            gpio.output(self.out_pins[i], value & (1 << i))

HOST = "0.0.0.0"
PORT = 9999
ENCODING = "ASCII"
count = 0
class RaspberryPiHandler(socketserver.BaseRequestHandler):
    """
    Handles one incoming datagram.
    """
    pipin = PiPin((37, 38, 40))
    action_list = ('STOP', 'FORWARD', 'BACKWARD', 'LEFT', 'RIGHT', 'SPEEDUP', 'SPEEDDOWN')
    def handle(self):
        logging.info("On socket({}) received \"{}\"".format(self.request[1], self.request[0]))
        logging.debug("type(self.request[0])={}".format(type(self.request[0])))
        datagram = self.request[0].decode(encoding=ENCODING)
        try:
            rx_cmd = RxCmd(datagram)
        except NotRxCmdException:
            logging.warn("Received illegal RxCmd: {}".format(datagram))
            return
        is_done = False
        for i in range(len(RaspberryPiHandler.action_list)):
            if rx_cmd.cmd == RaspberryPiHandler.action_list[i]:
                logging.debug("Do {}".format(rx_cmd.cmd))
                RaspberryPiHandler.pipin.write(i)
                is_done = True
        if not is_done:
            logging.warn("Operation unknown: {}".format(rx_cmd.cmd))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-v", "--verbosity", help="verbosity of logging output [0..4]", action="count", default=0)
    args = parser.parse_args()
    if args.verbosity > 4:
        args.verbosity = 4
    log_lvl = (logging.CRITICAL, logging.ERROR, logging.WARNING, logging.INFO, logging.DEBUG)[args.verbosity]
    logging.basicConfig(level=log_lvl, format='%(filename)s:%(levelname)s:%(message)s')
    logging.info("Starting UdpServer instance...")
    server = socketserver.UDPServer((HOST, PORT), RaspberryPiHandler)
    logging.info("UdpServer instance started.")
    logging.info("Let UdpServer to serve forever ...")
    server.serve_forever()
