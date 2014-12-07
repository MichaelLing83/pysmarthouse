#!/usr/bin/env python3
import socket
import sys

HOST, PORT = "192.168.31.107", 9999
# data = " ".join(sys.argv[1:])
data = "kitchen;temperature;-100"

# SOCK_DGRAM is the socket type to use for UDP sockets
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# As you can see, there is no connect() call; UDP has no connections.
# Instead, data is directly sent to the recipient via sendto().
sock.sendto(bytes(data, "ascii"), (HOST, PORT))
#received = str(sock.recv(1024), "utf-8")

print("Sent:     {}".format(data))
#print("Received: {}".format(received))
