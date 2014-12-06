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
'''
import socketserver

class UDPHandler(socketserver.BaseRequestHandler):

    def handle(self):
        data = self.request[0].strip()
        socket = self.request[1]
        # print("{} wrote:".format(self.client_address[0]))
        # print(data)
        socket.sendto(data.upper(), self.client_address)

if __name__ == "__main__":
    HOST, PORT = "", 9999   ## all interfaces
    server = socketserver.UDPServer((HOST, PORT), UDPHandler)
    server.serve_forever()