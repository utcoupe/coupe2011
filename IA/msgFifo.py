# -*- coding: utf-8 -*-

import threading
from Queue import Queue, Empty
from protocole import *

class Message:
	def __init__(self, id_from, id_msg, id_cmd, content):
		self.id_from = int(id_from)
		self.id_msg = int(id_msg)
		self.id_cmd = int(id_cmd)
		self.content = content

	def __repr__(self):
		return "(id_from=%s, id_msg=%s, id_cmd=%s, content=%s)"%(self.id_from, self.id_msg, self.id_cmd, self.content)

class MsgFifo:
	"""
	FIFO qui récupère les messages
	"""
	def __init__(self, *msg2wait):
		"""
		@param msg2wait (list<id_cmd>)
		"""
		self._queue = Queue()
		self._filter = [Q_KILL]
		self._filter += msg2wait
	
	def addMsg(self, id_from, id_msg, id_cmd, msg):
		"""
		ajouter un message à la queue
		"""
		if id_cmd in self._filter:
			m = (Message(id_from, id_msg, id_cmd, msg))
			self._queue.put(m)
	
	def getMsg(self, time=None, msg=""):
		"""
		@raise TimeoutException
		
		@param time (float) timeout en s
		
		@return (Message)
		"""
		try:
			r = self._queue.get(True, time)
		except Empty:
			raise TimeoutException("%s.getMsg(timeout=%s,msg=%s)"%(self,time,msg))
		else:
			self._queue.task_done()
			return r
	
	def __repr__(self):
		return "MsgFifo(%s)"%(tuple(get_cname(_) for _ in self._filter),)

	
	
	
	
