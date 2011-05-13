#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Création du server
ajout des ports serials
mise sur écoute de seulement eux même
identification et réarrangement
"""

from server import *

server = Server()

#server.addSubprocessClient("clients/python/UDPClient/main.py")
server.addSubprocessClient("./UTCamera");
#server.addSubprocessClient(["../../../IA/main.py","1","0"])

import glob

def scanSerials():
	pathname = '/dev/ttyACM*'
	return glob.iglob(pathname)


for serial in scanSerials():
	server.addSerialClient(serial,115200)

	
for client in server.clients:
	if client.id != ID_SERVER:
		client.blockRecv()
		client.setMaskRecvFrom(ID_SERVER,1) # les cartes ne peuvent recevoir des messages que du serveur
		client.setMaskRecvFrom(ID_IA,1) # et de l'IA
		

# le serveur ne démarre qu'après avoir connecté les serials
server.start()

print 'fin thread principal'





