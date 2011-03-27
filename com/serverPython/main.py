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
import socket


from server import *
from timer import *



scan = scanPorts()
	
ports = []
for s in scan:
	ports.append((s, 115200))
#ports.append(("../exe",None))
#ports.append((["./ObjectCamera","-b","-m","n"], None))
ports.append((["python","../../emulation/emulation.py","asserv"],None))

if not ports:
	print "rien à connecter..."
	exit()
	

server = Server(ports)
server.start()


print 'fin thread principal'





