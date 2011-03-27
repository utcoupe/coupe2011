# -*- coding: utf-8 -*

import socket
import subprocess
import re
import sys

sys.path.append('../../com/clients/pyClient')
import pyClient


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


print (host, port)


UDPSock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
UDPSock.bind((host, port))


def fn_input():
    data, a = UDPSock.recvfrom(1024)
    return str(data)

def fn_on_close():
    UDPSock.close()


HOST = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
PORT = sys.argv[2] if len(sys.argv) > 2 else 50000            # The same port as used by the server

client = pyClient.pyClient(HOST, PORT, fn_input, fn_on_close)
client.start()

print "Standby ready !"




