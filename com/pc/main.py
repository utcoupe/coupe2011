#!/usr/bin/env python
# -*- coding: utf-8 -*-

import threading
from message import *
from timer import *

"""
	commandes funky:
		live <cmd>: aficher une nouvelle fenetre avec en boucle un envoi/reception de la commande spécifiée
		stop <n>: killer le nième écran
		test <cmd>: lance 300 fois la commande et affiche le temps d'execution
"""


ports = []
ports.append(('ACM0',115200))
#ports.append(('ACM1',115200))

server = Server(ports)

# read, send and get output of a command
def loopCmd():
	cmd = raw_input()
	cperso = cmd.split()
	if not cmd:
		cmd = 'p'
	if cperso[0] == 'test':
		server.testPing('ACM0', cperso[1])
	elif cperso[0] == 'live':
		server.getLive('ACM0', cperso[1])
	elif cperso[0] == 'stop':
		server.stopScreen(int(cperso[1]))
	else:
		server.addCmd(cmd, 'ACM0')
		print 'vjuv',server.getRcv(cmd, 'ACM0', True)
	

def makeLoop(target, args= [], kwargs={}):
	while True:
		target(*args, **kwargs)
	

loopCmd = threading.Thread(None, makeLoop, None, (loopCmd,))
loopCmd.start()

time.sleep(60)



'''

on = True
def blink():
	global on
	if on:
		on = False
		ser['ACM0'].write(chr(2))
		print 'on'
	else:
		on = True
		ser['ACM0'].write(chr(3))
		print 'off'

timer = MyTimer(1, blink)
timer.start()
time.sleep(10)
timer.stop()
'''

"""
while True:
	ser['ACM0'].write('<Sd>')
	val = ser['ACM0'].readline()
	if val:
		print 'ACM0 : ', val # int(binascii.hexlify(val),16)  #unpack('c', val)
	else:
		print 'timeout'
"""

'''tot = 0
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
	ser['ACM0'].write('<LL>')
	for i in range(8):
		print 'ACM0 : '+ser['ACM0'].readline()
	t = time.time()-t
	print t
	tot += t
print tot
print tot/300.0

'''
