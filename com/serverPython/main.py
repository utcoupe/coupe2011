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
server.start()

for serial in scanSerials():
    server.addSerialClient(serial,115200)
    
for client in server.clients:
    if client.id != 0:
        client.blockRecv()
        client.setMaskRecvFrom(ID_SERVER) # les cartes ne peuvent recevoir des messages que du serveur
        client.setMaskRecvFrom(ID_IA) # et de l'IA
        


print 'fin thread principal'





