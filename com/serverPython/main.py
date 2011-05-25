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
server.start()

#server.addSubprocessClient("clients/python/UDPClient/main.py")
server.addSubprocessClient("./UTCamera")
server.addSubprocessClient("../../../pinceControl/AX12/scriptPince.py")
#server.addSubprocessClient(["../../../IA/main.py","1","0"])

import glob

def scanSerials():
	pathname = '/dev/ttyACM*'
	return glob.iglob(pathname)


for serial in scanSerials():
	server.addSerialClient(serial,115200)

		


print 'fin thread principal'





