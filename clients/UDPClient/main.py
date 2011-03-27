# -*- coding: utf-8 -*


import socket
import subprocess
import re
import sys
import threading


host = "";
port = 5555

p = subprocess.Popen("ifconfig", shell=True, stdout=subprocess.PIPE)
result = p.communicate()[0].strip().split("\n\n")

interfaces = list()
for bloc in result:
    r = re.search('^(\S+)\s', bloc)
    name = r.group(1)
    r = re.search('inet add?r:(\S+) ', bloc)
    ip = r.group(1) if r else ""
    r = re.search('(\d+.\d+) [KMG]?B', bloc)
    debit = float(r.group(1)) if r else 0.0
    interfaces.append((name,ip,debit))

# les wifi en premier
def fn_cmp(x,y):
    namex, ipx, debitx = x
    namey, ipy, debity = y
    if 'wlan' in namex:
        poidsx = 0
    else:
        poidsx = 1
    if 'wlan' in namey:
        poidsy = 0
    else:
        poidsy = 1

    return poidsx - poidsy
    
interfaces = sorted(interfaces, cmp=lambda x,y: fn_cmp(x,y))

for name,ip,debit in interfaces:
    #print name, ip, debit
    if name != 'lo' and ip and debit > 0.0:
        print "found :",name, ip, port
        host = ip
        break

if not host:
    host = raw_input("host : ")




bufsize = 1024
addr = (host,port)


UDPSock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
UDPSock.bind(addr)




if len(sys.argv) > 1:
    HOST = sys.argv[1]
else:
    HOST = raw_input("host? : ")    # The remote host

PORT = 50000            # The same port as used by the server
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

print "Standby ready !"

def fn_send():
    while True:
        data,addr = UDPSock.recvfrom(bufsize)
        msg = str(data)
        print msg
        s.send(msg)
        if msg == 'close' or msg == 'shutdown':
            print "break send"
            break

def fn_recv():
    while True:
        data = s.recv(1024) # en octets/chars
        if not data or str(data) == 'close':
            UDPSock.close()
            s.close()
            print "break recv"
            break
        print 'Received', repr(data)


t_send = threading.Thread(None, fn_send, None)
t_recv = threading.Thread(None, fn_recv, None)
t_send.start()
t_recv.start()
