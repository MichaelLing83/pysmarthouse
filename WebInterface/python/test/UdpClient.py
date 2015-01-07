#!/bin/env python3

import socket
import sys  #for exit

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.settimeout(3)
except socket.error:
    print('Failed to create socket')
    sys.exit()

host = 'localhost';
port = 9999;

while(1) :
    msg = input('Enter message to send : ')

    try :
        #Set the whole string
        s.sendto(msg.encode(encoding="ASCII"), (host, port))
        # receive data from client (data, addr)
        d = s.recvfrom(1024)
        reply = d[0]
        addr = d[1]
        print('Server reply : ' + reply.decode(encoding="ASCII"))
    except socket.timeout as msg:
        print("No response from server before timeout.")
    except socket.error as msg:
        print('Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()