# -*- coding: utf-8 -*-


import threading
import message
import time
from envoyeur import *
from receveur import *


class EventListener(threading.Thread):
	def __init__(self, event, name):
		threading.Thread.__init__(self, None, None, "EventListener(%s)"%str(name))
		self._event = event
		self._stop_event = threading.Event()
	
	def run(self):
		while not self._stop_event.isSet():
			self._event.wait(2)
			if self._event.isSet():
				self.action()
		print "%s arreté"%self.name
	
	def action(self):
		pass
	
	def stop(self):
		self._stop_event.set()
	
	def clear(self):
		self._event.clear()


class DisconnectListener(EventListener):
	def __init__(self, disconnect_event, reconnect_event, server, id_client):
		EventListener.__init__(self, disconnect_event, id_client)
		self._server = server
		self._id_client = id_client
		self._reconnect_event = reconnect_event
		
	def action(self):
		print "port %s deconnecté..."%str(self._id_client)
		scan = message.scanPorts("ttyACM")
		for port in scan:
			print self._server.ports
			if port not in self._server.ports.values() or port == self._server.ports[self._id_client]:
				id, client = self._server.connect(port, 115200)
				if id == self._id_client:
					self._server.associateThreads(id, client, self._event, self._reconnect_event)
					self._event.clear() # plus deconnecté
					self._reconnect_event.set() # prevenir que l'on a reconnecté
					break
				elif id:
					client.close()
			
			if self._event.isSet(): # si on a pas réussi à reconnecté on retentera dans 1 seconde
				time.sleep(1)
