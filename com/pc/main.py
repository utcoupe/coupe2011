#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
	commandes funky:
		loop <time> puis <cmd>: aficher une nouvelle fenetre avec en boucle un envoi/reception de la commande spécifiée
		stop <n>: killer le nième écran
		test <cmd>: lance 300 fois la commande et affiche le temps d'execution
		cam <num>: Recuperer les données de la nieme cam
		exit: quitter
"""


import threading
import sys

from server import *
from timer import *



scan = scanPorts()
if not scan:
	print "aucun port connecté..."
	exit()
	
	
ports = []
for s in scan:
	ports.append((s, 115200))
ports.append(("../exe",None))


server = Server(ports)


#
# début d'IA
#
"""
print "recalage..."
# se recaler
event = server.addCmd("x", "asserv")
event.wait(1)
print "Reponse :", server.getReponse("x", "asserv")
# avancer d'1m
event = server.addCmd("a 1 0", "asserv")
event.wait(1)
print "Reponse :", server.getReponse("a 1 0", "asserv")
print "on attend ~5 secondes"
for i in range(5):
	event = server.addCmd("k")
	event.wait(1)
	print "Reponse :", server.getReponse("x", "asserv")
	time.sleep(1)

# scan de la carte

"""



"""
# read, send and get output of a command
while True:
	cmd = raw_input()
	
	cmd_split = cmd.split(' ',1) # <carte><cmd>
	
	
	if not cmd:
		print 'P'
		cmd = 'P'
		cmd_split = ['P']
	if cmd_split[0] == 'test':
		server.testPing(port, cmd_split[1])
	elif cmd_split[0] == 'track':
		it = InteruptableThreadedLoop(None, 'tracking')
		it.start(loopTestTracking)
		c = raw_input()
		it.stop()
		print 'arret du thread tracking en cours...'
	elif cmd_split[0] == 'exit':
		server.stop()
		break
	elif cmd_split[0] == 'loop':
		cmd_split2 = raw_input().split(' ',1)
		server.makeLoop(cmd_split2[0], cmd_split2[1], cmd_split[1])
	elif cmd_split[0] == 'stop':
		server.stopScreen(int(cmd_split[1]))
	else:
		event = server.addCmd(cmd_split[1], cmd_split[0])
		event.wait(1)
		r = server.getReponse(cmd_split[1], cmd_split[0])
		print "Reponse :", r
"""

print 'fin thread pcincipal'





