#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import threading
import time

ser = dict()

def connect(port, refresh):
	global ser
	for i in range(10):
		try:
			#print 'tentative de connection sur %s'%port
			s = serial.Serial('/dev/tty'+port, refresh, timeout=1, writeTimeout=1)
		except serial.SerialException:
			print 'connection echouée sur %s, nouvelle tentative'%port
		else:
			print 'connection %s établie'%port
			ser[port] = s
			return s
	print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
	exit()


a = threading.Thread(None, connect, None, ('ACM0',9600,))
#b = threading.Thread(None, connect, None, ('ACM1',115200))
a.start()
#b.start()

while a.isAlive():# or b.isAlive():
	time.sleep(1)

time.sleep(2)

while True:
	ser['ACM0'].write(chr(1))
	val = ser['ACM0'].readline()
	if val:
		print 'ACM0 : '+val
	else:
		print 'timeout'
	
"""
tot = 0
for i in xrange(300):
	t = time.time()
	ser['ACM0'].write(chr(1))
	val = ser['ACM0'].readline()
	if val:
		print 'ACM0 : '+val
		t = time.time()-t
		print t
		tot += t
	else:
		print 'timeout'
print tot
print tot/300.0

tot = 0
for i in xrange(300):
	t = time.time()
	ser['ACM1'].write('<?>')
	for i in range(8):
		print 'ACM1 : '+ser['ACM1'].readline()
	t = time.time()-t
	print t
	tot += t
print tot
print tot/300.0
"""

