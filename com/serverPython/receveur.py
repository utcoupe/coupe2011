# -*- coding: utf-8 -*-


import threading
import serial
from protocole import *


class Receveur(threading.Thread):
	""" Thread qui va lire en permanence sur la sortie de la carte
	"""
	def __init__(self, id_device, device, disconnect_event, reconnect_event, clients):
		"""
		@param id_device l'identifiant du device (récupérer après une demande d'identification '<I>')
		@param device le serial ou subprocess associé
		@param disconnect_event desactivé en temps normal, activé pendant une deconnection
		@param reconnect_event activé quand la reconnection a réussie
		@param clients la liste qui contient les clients du serveur
		"""
		threading.Thread.__init__(self, None, None, "Receveur(%s)"%str(id_device))
		self._kill_event = threading.Event()
		self._id_device = id_device
		self._device = device
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
		self._clients = clients
		
	def run(self):
		while not self._kill_event.isSet():
			self._loop()
			self._kill_event.wait(0.0001)
		print "%s arreté"%self.name
			
	def kill(self):
		self._kill_event.set()
		self._device.close()
	
	def _readLine(self):
		"""
		@return
			None si rien, timeout ou deconnection
			(string) si on a réussi à lire
		"""
		try:
			val = self._device.readline()
		except serial.SerialException as ex:
			print ex
		except OSError as ex:
			print "%s : (ERROR) deconnection..."%self.name
			self._disconnect_event.set()
			self._reconnect_event.clear()
			return None
		except IOError as ex:
			print "%s : (ERROR) %s"%(self.name,ex)
			self._disconnect_event.set()
			self._reconnect_event.clear()
			return -1
		else:
			val = val.replace("§","\n")
			if val:
				return val # int(binascii.hexlify(val),16)  #unpack('c', val)
	
	def _loop(self):
		self._reconnect_event.wait()
		r = self._readLine()
		if r:
			for c in self._clients:
				if c: c.send("%s %s"%(self._id_device,r))
			#print r

		