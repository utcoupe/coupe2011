#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Création du server
ajout des ports serials
mise sur écoute de seulement eux même
identification et réarrangement
"""

import os
ROOT_DIR  = os.path.split(os.path.split(os.path.dirname(os.path.abspath(__file__)))[0])[0]
print ROOT_DIR

import subprocess

from server import *

server = Server()
server.start()

#server.addSubprocessClient("clients/python/UDPClient/main.py")
server.addSubprocessClient(os.path.join(ROOT_DIR,"Visio","UTCamera","bin","UTCamera"))
server.addSubprocessClient(os.path.join(ROOT_DIR,"pinceControl","AX12","scriptPince.py"))
#server.addSubprocessClient(["../../../IA/main.py","1","0"])
p = subprocess.Popen(os.path.join(ROOT_DIR,"smartphone.py")) #, stdout=subprocess.PIPE)


import glob

def scanSerials():
	pathname = '/dev/ttyACM*'
	return glob.iglob(pathname)


for serial in scanSerials():
	server.addSerialClient(serial,115200)

server.parseMsg(ID_SERVER, "ls")

p.join()

print 'fin thread principal'




