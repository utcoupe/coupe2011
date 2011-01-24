#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial

def connect(port):
	for i in range(10):
		try:
			ser = serial.Serial('/dev/tty'+port, 115200)
		except serial.SerialException:
			print 'connection echouée sur %s, nouvelle tentative'%port
		else:
			print 'connection %s établie'%port
			return ser
	print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
	return False

ser0 = connect('ACM0')
ser1 = connect('ACM1')
if ser0:
	print ser0.readline()
if ser1:
	print ser1.readline()
