#!/usr/bin/env python3
'''
All clients have to follow this format when sending to this server:
    1. Use UDP
    2. Port 9999
    3. each payload format (as a string coded in ASCII):
        ID;type;value
            ID: identity of the sender, e.g. "kitchen_controller"
            type: type of the report, e.g. "temperature", "light", "infrared"
            value: integer in unit of 0.001
        Exemples:
            "frontyard_controller;temperature;-1900"    ->  frontyard_controller sent us with temperature of -19.00 degree Celsius

    Database:
        Name: pysmarthouse.db
        Database: sqlite3
        Table name: pysmarthouse
        Table format: (Time text, ID text, Type text, Value real)
'''
import socketserver
import sqlite3
import datetime

class UDPHandler(socketserver.BaseRequestHandler):

    def __init__(self):
        super.__init__(self)
        self.conn = sqlite3.connect("./pysmarthouse.db")
        self.c = self.conn.cursor()

    def handle(self):
        data = self.request[0].strip()
        socket = self.request[1]
        #socket.sendto(data.upper(), self.client_address)
        _id, _type, _value = data.split(';')
        _value = float(_value) / 100
        _time = str(datetime.datetime.utcnow())
        self.c.execute("INSERT INTO pysmarthouse VALUES (_time, _id, _type, _value)")
        

if __name__ == "__main__":
    HOST, PORT = "", 9999   ## all interfaces
    server = socketserver.UDPServer((HOST, PORT), UDPHandler)
    server.serve_forever()