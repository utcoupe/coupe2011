#!/usr/bin/env python
# -*- coding: utf-8 -*-



from server import *


server = Server()
server.start()

server.addSerialClient('/dev/ttyACM0',115200)

print 'fin thread principal'





