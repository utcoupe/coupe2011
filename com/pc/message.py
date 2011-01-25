# -*- coding: utf-8 -*-



#from struct import *
#import binascii
import serial
import threading
import time



class Server():
	def __init__(self, ports):
		self.ser = dict()
		
		print ports
		
		# lancement des tentatives de connection
		threads = []
		for port,refresh in ports:
			print port,refresh
			thread = threading.Thread(None, self.connect, None, (port,refresh,))
			thread.start()
			threads.append(thread)
		
		# attendre que les connections soient établies
		while True:
			for t in threads:
				if t.isAlive():
					time.sleep(2)
					break # break que le for
			break
		
		# Verification que les connections ont reussi
		for s in self.ser.values():
			if not s: 
				exit()

		# petite pause avant de pouvoir envoyer et recevoir des données
		time.sleep(2)
	
	
	def connect(self, port, refresh):
		for i in range(10):
			try:
				#print 'tentative de connection sur %s'%port
				s = serial.Serial('/dev/tty'+port, refresh, timeout=1, writeTimeout=1)
			except serial.SerialException:
				print 'connection echouée sur %s, nouvelle tentative'%port
			else:
				print 'connection %s établie'%port
				self.ser[port] = s
				return s
		print 'echec de la connection %s après 10 tentatives, relancer le programme'%port
		self.ser[port] = None
		return self.ser[port]
	
	"""
		commande brute (sans check sum ni caractere debut/fin
	"""
	def sendCmd(self, cmd):
		self.ser['ACM0'].write('<'+cmd+'>')
	
	
	
	def readInput(self, port):
		val = self.ser[port].readline()
		val = val.replace("§","\n")
		if val:
			return port + ':' + val # int(binascii.hexlify(val),16)  #unpack('c', val)
		else:
			return 'timeout on :',port
			
