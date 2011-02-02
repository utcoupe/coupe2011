# -*- coding: utf-8 -*-


import threading
import serial
from Queue import *

class Envoyeur(threading.Thread):
	def __init__(self, id_client, client, disconnect_event, reconnect_event):
		threading.Thread.__init__(self, None, None, "Envoyeur(%s)"%str(id_client))
		self._stopevent = threading.Event()
		self._client = client
		self._queue = Queue()
		self._disconnect_event = disconnect_event
		self._reconnect_event = reconnect_event
	
	def addCmd(self, cmd):
		self._queue.put(cmd)		
		
	def run(self):
		while not self._stopevent.isSet():
			self._loop()
			self._stopevent.wait(0.0001)
		print "%s arreté"%self.name
			
	def stop(self):
		self._stopevent.set()
	
	def _sendCmd(self, cmd):
		try:
			self._client.write('<'+cmd+'>')
			return 1
		except serial.SerialException as ex:
			raise Exception, "timeout, writing on"+str(self._client)
		except OSError as ex:
			print "deconnection..."
			self._disconnect_event.set()
	
	def _loop(self):
		self._reconnect_event.wait()
		try:
			cmd = self._queue.get(True, 2)
		except Empty:
			return
		r = self._sendCmd(cmd)
		self._queue.task_done()
