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
		self._stop_event = threading.Event()
		self._verrou = verrou
	
	def run(self):
		while not self._stop_event.isSet():
			self._event.wait(2)
			if self._event.isSet():
				self._verrou.acquire()
				self.action()
				self._verrou.release()	
		print "%s arreté"%self.name
	
	def action(self):
		""" à surcharger, fonction appellée quand l'event se produit """
		pass
	
	def stop(self):
		self._stop_event.set()
	
	def clear(self):
		self._event.clear()


class DisconnectListener(EventListener):
	""" Catch les event de deconection et tente une reconnection """
	def __init__(self, disconnect_event, reconnect_event, server, id_client, verrou):
		EventListener.__init__(self, disconnect_event, id_client, verrou)
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
		self._server = server
		self._id_client = id_client
		
	def action(self):
		print "%s : carte '%s' on port %s deconnected..."%(self.name, str(self._id_client), self._server.ports[self._id_client])
		port_actuel = self._server.ports[self._id_client]
		self._server.ports_connection[port_actuel] = False
		scan = server.scanPorts("ttyACM")
		for port in scan:
			# si le port n'est pas enregistré dans le serveur ou qu'il est indiqué inactif
			if port not in self._server.ports_connection or not self._server.ports_connection[port]:
				# alors on tente une connection
				id_client, client = self._server.connect(port, 115200, False)
				if id_client:
					self._server.envoyeurs[id_client]._client = client
					self._server.receveurs[id_client]._client = client
					if port == self._server.ports[self._id_client]:
						self._event.clear() # plus deconnecté
						self._reconnect_event.set() # prevenir que l'on a reconnecté
		
		# si on a pas réussi à reconnecter notre port on fait une pause
		if self._event.isSet():
			time.sleep(1)
