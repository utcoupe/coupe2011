# -*- coding: utf-8 -*-

import sys
import socket
import threading


if len(sys.argv) > 1:
    HOST = sys.argv[1]
else:
    HOST = raw_input("host? : ")    # The remote host

PORT = 50006              # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))


def fn_send():
    while True:
        msg = raw_input()
        if 'exit' == msg:
            s.close()
            exit()
        else:
            s.send(msg)

def fn_recv():
    while True:
        data = s.recv(1024) # en octets/chars
        print 'Received', repr(data)

threading.Thread(None, fn_send, None).start()
threading.Thread(None, fn_recv, None).start()





