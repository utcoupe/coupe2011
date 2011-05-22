
import subprocess


from debug.main import *
from protocole import *


class Debug:
	def __init__(self, lvl):
		self.lvl = lvl
		if lvl != 0:
			self.p = subprocess.Popen("debug/main.py", stdout=subprocess.PIPE, stdin=subprocess.PIPE)
	
	def log(self, id_cmd, args=None):
		if self.lvl != 0:
			msg = str(id_cmd)+C_SEP_SEND+str(args)
			print "debug log : '%s'"%msg
			self.p.stdin.write(msg+"\n")
	
	def __del__(self):
		if self.lvl != 0:
			self.p.terminate()
			self.p.kill()
