# -*- coding: utf-8 -*-

import time
import sys
import pyClient


HOST = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
PORT = int(sys.argv[2]) if len(sys.argv) > 2 else 50000			# The same port as used by the server


debut = 0
def fn_input():
	global debut
	msg = raw_input()
	if 'test' in msg:
		debut = time.time()
		return msg.split(' ',1)[1]
	else:
		return msg

def fn_output(msg):
	global debut
	if debut:
		sys.stdout.write(str((time.time()-debut)*1000)+"ms\n")
		debut = 0
	sys.stdout.write(msg)
	
client = pyClient.pyClient(HOST, PORT, fn_output, fn_input)
client.start()


