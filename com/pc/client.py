# -*- coding: utf-8 -*-


class ClientSerial():
	def __init__(self, serial, origin, baudrate):
		self._serial = serial
		self.origin = origin
		self.baudrate = baudrate
		
	def write(self, msg):
		self._serial.write(str(msg)+"\n")
		
	def readline(self):
		return self._serial.readline()
	
	def __str__(self):
		return "ClientSerial(%s)"%self.origin
	
	def close(self):
		self._serial.close()


class ClientSubprocess():
	def __init__(self, subprocess, origin, baudrate):
		self._subprocess = subprocess
		self.origin = origin
		self.baudrate = baudrate
	
	def write(self, msg):
		self._subprocess.stdin.write(str(msg)+"\n") # envoie au child
		self._subprocess.stdin.flush()
	
	def readline(self):
		return self._subprocess.stdout.readline()
	
	def __str__(self):
		return "ClientSubprocess(%s)"%self.origin
	
	def close(self):
		self._subprocess.kill()
