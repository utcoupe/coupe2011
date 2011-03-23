# -*- coding: utf-8 -*

import serial
import socket
import subprocess
import re
import sys



host = "";
port = int(sys.argv[1]) if len(sys.argv)>1 else 5555

p = subprocess.Popen("ifconfig", shell=True, stdout=subprocess.PIPE)
result = p.communicate()[0].strip().split("\n\n")

interfaces = dict()
for bloc in result:
	r = re.search('^(\S+)\s', bloc)
	name = r.group(1)
	r = re.search('inet add?r:(\S+) ', bloc)
	ip = r.group(1) if r else ""
	r = re.search('(\d+.\d+) [MG]?B', bloc)
	debit = float(r.group(1))
	interfaces[name] = [ip,debit]
for name,(ip,debit) in interfaces.items():
	#print name, ip, debit
	if name != 'lo' and ip and debit > 0.0:
		print "found :",name, ip, port
		host = ip

if not host:
	host = raw_input("host : ")


#usb = "/dev/ttyACM0"
#ser = serial.Serial(usb, 115200)

bufsize = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((host, port))
s.listen(1)
while 1:
	conn, addr = s.accept()
	print 'Connected by', addr
	while 1:
		data = conn.recv(1024)
		if not data: break
		print data
	conn.close()


