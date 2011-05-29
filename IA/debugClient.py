
import subprocess


from debug.main import *
from protocole import *


class Debug:
	def __init__(self, path_exec, lvl):
		self.lvl = lvl
		print path_exec
		if lvl != 0:
			self.p = subprocess.Popen(path_exec, stdout=subprocess.PIPE, stdin=subprocess.PIPE)
	
	def log(self, id_cmd, args=None):
		if self.lvl != 0:
			msg = str(id_cmd)+C_SEP_SEND+str(args)
			#print "debug log : '%s'"%msg
			self.p.stdin.write(msg+"\n")
	
	def __del__(self):
		if self.lvl != 0:
			self.p.terminate()
			self.p.kill()
