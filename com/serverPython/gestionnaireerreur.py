# -*- coding: utf-8 -*-


import threading
import server
import time
from envoyeur import *
from receveur import *




class EventListener(threading.Thread):
	def __init__(self, event, name, verrou):
		threading.Thread.__init__(self, None, None, "EventListener(%s)"%str(name))
		self._event = event
		self._kill_event = threading.Event()
	
	def run(self):
		while not self._kill_event.isSet():
			self._event.wait(2)
			if self._event.isSet():
				self.action()
		print "%s arreté"%self.name
	
	def action(self):
		""" à surcharger, fonction appellée quand l'event se produit """
		raise NotImplementedError
	
	def kill(self):
		self._kill_event.set()
	
	def clear(self):
		self._event.clear()


class DisconnectListener(EventListener):
	""" Catch les event de deconection et tente une reconnection """
	def __init__(self, disconnect_event, reconnect_event, server, id_device, verrou):
		EventListener.__init__(self, disconnect_event, id_device, verrou)
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
		self._server = server
		self._id_device = id_device
		self._device = server.devices[id_device]
		self._verrou = verrou
		
	def action(self):
		self._verrou.acquire()
		print "%s : '%s' on port %s deconnected..."%(self.name, str(self._id_device), self._server.ports[self._id_device])
		port_actuel = self._server.ports[self._id_device]
		self._server.ports_connection[port_actuel] = False
		# pour les ports série
		if self._device.baudrate:
			scan = server.scanPorts()
			for port in scan:
				# si le port n'est pas enregistré dans le serveur ou qu'il est indiqué inactif
				if port not in self._server.ports_connection or not self._server.ports_connection[port]:
					# alors on tente une connection
					id_device, device = self._server.connect(port, 115200, False)
					self._postConnection(port,id_device,device)
		# pour la cam
		else:
			id_device, device = self._server.connect(self._device.origin, None)
			self._postConnection(self._device.origin,id_device,device)
		
		
		# si on a pas réussi à reconnecter notre port on fait une pause
		if self._event.isSet():
			time.sleep(1)
		self._verrou.release()

	def _postConnection(self, port, id_device, device):
		""" met à jour Envoyeur et Receveur
		met à jour les event deconnect et reconnect
		"""
		if id_device:
			self._server.envoyeurs[id_device]._device = device
			self._server.receveurs[id_device]._device = device
			#print port, self._server.ports[self._id_device]
			if str(port) == self._server.ports[self._id_device]:
				self._event.clear() # plus deconnecté
				self._reconnect_event.set() # prevenir que l'on a reconnecté
		
