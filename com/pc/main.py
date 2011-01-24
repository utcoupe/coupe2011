#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import threading
import time

ser = dict()

def connect(port):
	global ser
	for i in range(10):
		try:
			#print 'tentative de connection sur %s'%port
			s = serial.Serial('/dev/tty'+port, 115200, timeout=1, writeTimeout=1)
		except serial.SerialException:
			print 'connection echouée sur %s, nouvelle tentative'%port
		else:
			print 'connection %s établie'%port
			ser[port] = s
			return s
	print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
	exit()


a = threading.Thread(None, connect, None, ('ACM0',))
b = threading.Thread(None, connect, None, ('ACM1',))
a.start()
b.start()

while a.isAlive() or b.isAlive():
	time.sleep(1)

time.sleep(2)

v = input()

ser['ACM0'].write(chr(v))
print 'ACM0 : '+ser['ACM0'].readline()
#print ser['ACM1'].readline()
ser['ACM1'].write('<?>')
#ser['ACM1'].write('<?>')
#ser['ACM1'].write('<?>')
for i in range(10):
	print 'ACM1 : '+ser['ACM1'].readline()
	


ser['ACM0'].write(chr(v))
print 'ACM0 : '+ser['ACM0'].readline()
#print ser['ACM1'].readline()
ser['ACM1'].write('<?>')
#ser['ACM1'].write('<?>')
#ser['ACM1'].write('<?>')
for i in range(10):
	print 'ACM1 : '+ser['ACM1'].readline()


