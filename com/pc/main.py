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
			s = serial.Serial('/dev/tty'+port, 115200)
		except serial.SerialException:
			print 'connection echouée sur %s, nouvelle tentative'%port
		else:
			print 'connection %s établie'%port
			ser[port] = s
	print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
	exit()


a = threading.Thread(None, connect, None, ('ACM0',))
b = threading.Thread(None, connect, None, ('ACM1',))
a.start()
b.start()

while a.isAlive() or b.isAlive():
	time.sleep(1)

print ser['ACM0'].readline()
print ser['ACM1'].readline()
ser['ACM1'].write('<?>\n')
for i in range(50):
	print ser['ACM1'].readline()


