# -*- coding: utf-8 -*-

import time
import threading

class LoopCmd(threading.Thread):
    def __init__(self, robot, timeBeforeStart, interval, id_device, id_cmd, *args):
	"""
	@param robot (Robot)
	@param timeBeforeStart (float) temps avant le premier lancement de la cmd en s
	@param interval (float) interval entre chaque appelle en s
	@param id_device (int)
	@param id_cmd (int)
	@param *args les arguments pour la commande
	"""
	threading.Thread.__init__(self,None, None, "LoopCmd(%s,%s,%s,%s)"%(interval,id_device,id_cmd,args))
	self.robot = robot
	self.timeBeforeStart = timeBeforeStart
	self.interval = interval
	self._device = id_device
	self._cmd = id_cmd
	self._args = args
	
	self._e_stop = threading.Event()
	self._e_end = threading.Event()
    
    def stop(self):
	self._e_stop.set()
    
    def run(self):
	self._e_stop.wait(self.timeBeforeStart)
	while not self._e_stop.is_set():
	    start = time.time()
	    self.robot.addCmd(self._device, self._cmd, *self._args)
	    self._e_stop.wait(self.interval)
			
    def join(self, timeout=None):
	if self.isAlive():
	    self.robot.write('join')
	    threading.Thread.join(self, timeout)
