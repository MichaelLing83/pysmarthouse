#!/bin/env python3
'''
UdpServer class that receives all packets from Arduino's and update database.

All clients have to follow this format when sending to this server:
    1. Use UDP
    2. Port 9999
    3. each payload format (as a string coded in ASCII):
        ID;type;value
            ID: identity of the sender, e.g. "Kitchen"
            type: type of the report, e.g. "Temperature", "Relay"
            value: integer in unit of 0.001
        Exemples:
            "Kitchen;Temperature;-1900"    ->  Kitchen Arduino sent us with temperature of -19.00 degree Celsius
'''

import db
import socket

UDP_IP = "0.0.0.0"
UDP_PORT = 9999

class UdpServer:
    def __init__(self, debug=False):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind( (UDP_IP, UDP_PORT) )
        self.database = db.DB()
        self.debug = debug
    def run(self):
        while True:
            data, addr = sock.recvfrom(1024)
            if self.debug: print("data = ", data, "addr = ", addr)
            data = data.decode(encoding="ASCII")
            id, t, v = data.split(';')
            v = float(v.strip()) / 100
            if self.debug: print("id={0}, t={1}, v={2}".format(id, t, v))
            self.database.insert(id, t, v)

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--debug", help="Enable debug output", action="store_true")
    args = parser.parse_args()
    udp_server = UdpServer(args.debug)
    udp_server.run()
