#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
	commandes funky:
		live <cmd>: aficher une nouvelle fenetre avec en boucle un envoi/reception de la commande spécifiée
		stop <n>: killer le nième écran
		test <cmd>: lance 300 fois la commande et affiche le temps d'execution
"""


import threading
import sys

from message import *
from timer import *


ports = []
ports.append(('ACM0',115200))
#ports.append(('ACM1',115200))

server = Server(ports)

class Ordre(self):
	def __init__(self, cmd, fonction, isAliveCmd):
		self.cmd = cmd
		self.fonction = fonction
		self.timeSinceLastReponse = 0
	
	""" regarde si il y a un retour """
	def check(self):
		server.getRcv, (cmd, 'ACM0'
		

class IA():
	def __init__(self):
		self.cmdRunning = dict() # les commandes en cours
		self.errors		= dict() # les erreurs reçues
	
	def allerA(self):
	
	
	
	
# read, send and get output of a command
def loopCmd():
	cmd = raw_input()
	cperso = cmd.split()
	try:
		if not cmd:
			cmd = 'p'
		if cperso[0] == 'test':
			server.testPing('ACM0', cperso[1])
		elif cperso[0] == 'live':
			server.getLive('ACM0', cperso[1], cperso[2])
		elif cperso[0] == 'stop':
			server.stopScreen(int(cperso[1]))
		else:
			server.addCmd(cmd, 'ACM0')
			r = timeout(1.0, server.getRcv, (cmd, 'ACM0', True,))
			print r
	except Exception as ex:
		print sys.exc_info()
	

def makeLoop(target, args= [], kwargs={}):
	while True:
		target(*args, **kwargs)
	

loopCmd = threading.Thread(None, makeLoop, None, (loopCmd,))
loopCmd.start()

time.sleep(60)


