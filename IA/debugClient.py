
import subprocess


from debug.main import *
import sys
sys.path.append('../../com/serverPython/')
from protocole import *


class Debug:
	def __init__(self):
		self.p = subprocess.Popen("debug/main.py", stdout=subprocess.PIPE, stdin=subprocess.PIPE)
	
	def log(self, id_cmd, args=None):
		msg = str(id_cmd)+C_SEP_SEND+str(args)
		print "debug log : '%s'"%msg
		self.p.stdin.write(msg+"\n")
	
	def __del__(self):
		self.p.terminate()
		self.p.kill()
