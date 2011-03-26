# -*- coding: utf-8 -*-

import sys
import socket
import threading


if len(sys.argv) > 1:
    HOST = sys.argv[1]
else:
    HOST = raw_input("host? : ")    # The remote host

PORT = 50000            # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))


def fn_send():
    while True:
        msg = raw_input()
        s.send(msg)
        if msg == 'close' or msg == 'shutdown':
            print "break send"
            break

def fn_recv():
    while True:
        data = s.recv(1024) # en octets/chars
        if not data or str(data) == 'close':
            s.close()
            print "break recv"
            break
        print 'Received', repr(data)


t_send = threading.Thread(None, fn_send, None)
t_recv = threading.Thread(None, fn_recv, None)
t_send.start()
t_recv.start()





