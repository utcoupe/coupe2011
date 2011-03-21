# -*- coding: utf-8 -*

import serial
from socket import *
import subprocess
import re

host = "";

p = subprocess.Popen("ifconfig", shell=True, stdout=subprocess.PIPE)
result = p.communicate()[0].strip().split("\n\n")

interfaces = dict()
for bloc in result:
	r = re.search('^(\S+)\s', bloc)
	name = r.group(1)
	r = re.search('inet adr:(\S+) ', bloc)
	ip = r.group(1) if r else ""
	r = re.search('(\d+.\d+) [MG]?B', bloc)
	debit = float(r.group(1))
	interfaces[name] = [ip,debit]
for name,(ip,debit) in interfaces.items():
	if name != 'lo' and ip and debit > 0.0:
		print "found :",name, ip
		host = ip

if not host:
	host = raw_input("host : ")


usbdev = "/dev/ttyACM0"
#ser = serial.Serial(usbdev, 115200)

port = 5555
bufsize = 1024
addr = (host,port)

print addr

UDPSock = socket(AF_INET,SOCK_DGRAM)
UDPSock.bind(addr)

print "Standby ready !"

while 1:
    data,addr = UDPSock.recvfrom(bufsize)
    print data
    #ser.write(buf)

UDPSock.close()
#ser.close()


