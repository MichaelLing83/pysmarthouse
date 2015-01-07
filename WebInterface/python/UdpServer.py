#!/bin/env python3
'''
UdpServer class that receives all packets from Arduino's and update database.

All clients have to follow this format when sending to this server:
    1. Use UDP
    2. Port 9999
    3. Communication protocol (version 0.1):
        1) syntax (all letters are case sensitive)
            All fields are separated by a ";", and usage of ";" within any field is forbidden. A datagram must be ended with an ";". Each
            field is a ASCII string. The first field is <operation>, which must be one of "REPORT" or "CMD".
            When <operation>="REPORT", datagram is composed of 1+1+2*n (n=1,2,...) fields in order of "REPORT", <ID>, (<type>, <value>)+:
                <ID>: identity of the client, usually a geometry location name of an Arduino, e.g. "KitchenWindow", "FrontYard"
                <type>: type of report, i.e. what the <value> field in this report refers to, e.g. "Temperature", "InternetConnectivity"
                <value>: integer in unit of 0.001, e.g. "-1800" means -18.00
                Example: "REPORT;KitchenWindow;Temperature;2300;Relay;100", means the sender "KitchenWindow" reports a temperature at 23.00
                degree Celsius and its relay at value 1.00 (ON).
            When <operation>="CMD", the datagram is composed of 1+1*n (n=0,1,2,...) fields in strict order of "CMD", (<cmd>)+:
                <cmd>: the command receiver should execute.
                Example: "CMD;relay_on();delay(1000);relay_off()", means after receiving this datagram the receiver should execute
                "relay_on()" and then "delay(1000)" and then "relay_off()".
        2) semantics
            client -> server: only "REPORT" operation is supported, e.g. "REPORT;Doorstep;Temperature;-1900", means client named "Doorstep"
            reports a temperature at -19.00 degree Celsius. How server uses the content of this report is completely up to the server.
            server -> client: only "CMD" operation is supported, e.g. "CMD;reset".
        3) timing
            A TIMEOUT of 4 seconds is applied to each receiving attempt, if no response is received before timeout, operator should just
            reset its status and try again later (ideally a counter of consecutive timeout should also be maintained).
            Server should always listen, and any operation to handle a incoming datagram should take no more than TIMEOUT/2 in time. Server
            only responses to incoming datagram and then sends a datagram as response.
            Client sends one REPORT datagram to server every PERIOD (=60 second) or when an significant event happened, e.g. LightSensor
            threshold is reached, etc. When client sends a report, it waits for an response which is a "CMD" datagram, and executes <cmd>
            in the datagram.
'''
import logging
import db
import socketserver
import argparse

HOST = "0.0.0.0"
PORT = 9999
ENCODING = "ASCII"

class RaspberryPiHandler(socketserver.BaseRequestHandler):
    """
    Handles one incoming datagram.
    """
    def handle(self):
        logging.info("On socket({}) received \"{}\"".format(self.request[1], self.request[0]))
        logging.debug("type(self.request[0])={}".format(type(self.request[0])))
        datagram = self.request[0].decode(encoding=ENCODING).strip().split(';')
        for i in range(datagram.count('')):
            datagram.remove('') # remove empty items
        relay = 0
        # check if incoming datagram is legal
        if len(datagram) < 1 + 1 + 2:   # minimum datagram: "<operation>;<ID>;<type>;<value>"
            logging.warning("Incoming datagram is too short (len={})! {}".format(len(datagram), ';'.join(datagram)))
        elif datagram[0] != "REPORT":   # only <operation>="REPORT" is supported for client->server communication
            logging.debug("<operation>={}".format(datagram[0]))
            logging.warning("Incoming datagram has unsupported <operation>=\"{}\"! {}".format(datagram[0], ';'.join(datagram)))
        elif (len(datagram) - 2) % 2 != 0:  # <type>;<value> are pairs
            logging.debug(datagram)
            logging.warning("Incoming datagram has wrong number of fields (len={})! {}".format(len(datagram), ';'.join(datagram)))
        else:   # basic check passed
            logging.info("Incoming datagram: {}".format(';'.join(datagram)))
            operation, id = datagram[0:2]
            type_value_list = datagram[2:]
            for i in range(0, len(type_value_list), 2):
                db.DB().insert(id, type_value_list[i], float(type_value_list[i+1])/100)
                if type_value_list[0] == "Relay":
                    relay = float(type_value_list[1]) / 100
        # send a CMD datagram back
        # TODO: add real logic, for now only empty cmd is sent.
        if relay:
            cmd = "relay_off();"
        else:
            cmd = "relay_on();"
        socket = self.request[1]
        cmd = "CMD;{}".format(cmd)
        socket.sendto(cmd.encode(encoding=ENCODING), self.client_address)
        logging.info("Sent \"{}\"".format(cmd))

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
