#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Création du server
ajout des ports serials
mise sur écoute de seulement eux même
identification et réarrangement
"""

import glob

def scanSerials():
    pathname = '/dev/ttyACM*'
    return glob.iglob(pathname)

from server import *

server = Server()

for serial in scanSerials():
    server.addSerialClient(serial,115200)
    
for client in server.clients:
    if client.id != 0:
        client.blockRecv()
        client.setMaskRecvFrom(ID_SERVER,1) # les cartes ne peuvent recevoir des messages que du serveur
        client.setMaskRecvFrom(ID_IA,1) # et de l'IA
        

# le serveur ne démarre qu'après avoir connecté les serials
server.start()

print 'fin thread principal'





