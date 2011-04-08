# -*- coding: utf-8 -*-

import random
import threading
import serial


from sender import *
from protocole import *
from client import *
from tcpLoop import *


class Server():
	"""
	Cette classe permet la communication avec les différents ports série
	
	@author Thomas
	"""
	def __init__(self):
		self.clients = []
		self.sender = Sender(self.clients)
		#self.tcpLoop = TCPLoop(self,'', random.randint(40000,60000))
		self.tcpLoop = TCPLoop(self,'', 50000)
		self.e_shutdown = threading.Event()
		self._lock_write = threading.Lock()
		
		locClient = LocalClient(self, 0)
		locClient.start()
		self.clients.append(locClient)
	
	def start(self):
		self.tcpLoop.start()
	
	def stop(self):
		self.e_shutdown.set()
	
	def addTCPClient(self, conn):
		client = TCPClient(self,len(self.clients),conn)
		client.start()
		self.clients.append(client)
	
	def addSerialClient(self, port, baudrate):
		s = serial.Serial(port, baudrate, timeout=1, writeTimeout=1)
		client = SerialClient(self, len(self.clients), s, port, baudrate)
		client.start()
		self.clients.append(client)
	
	def addSubprocessClient(self, exec_name):
		process = subprocess.Popen(port, shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
		
	def parseMsg(self, id_client, msg):
		"""
		parse un message avant de l'envoyer
		"""
		msg = msg.strip()
		id_to, msg = msg.split(C_SEP_SEND,1)
		id_to = int(id_to)
		mask_from = (1 << id_client)
		mask_to = -1 if id_to == -1 else (1 << id_to)
		msg = str(id_client)+C_SEP_SEND+msg
		self.sender.send(mask_from, mask_to, msg)
	
	def write(self, msg):
		self._lock_write.acquire()
		print str(msg).strip()
		self._lock_write.release()
