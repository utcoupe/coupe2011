# -*- coding: utf-8 -*-


import threading

class LoopCmd(threading.Thread):
	def __init__(self, robot, interval, id_device, id_cmd, *args):
	    """
	    @param robot (Robot)
	    @param interval (float) interval entre chaque appelle en s
	    @param id_device (int)
	    @param id_cmd (int)
	    @param *args les arguments pour la commande
	    """
	    threading.Thread.__init__(self,None, None, "LoopCmd(%s,%s,%s,%s)"%(interval,device,cmd,args))
	    self.robot = robot
	    self.interval = interval
	    self._device = device
	    self._cmd = cmd
	    self._args = args
	    
	    self._e_stop = threading.Event()
	
	def stop(self):
		self._e_stop.set()
	
	def run(self):
		while not self._e_stop.is_set():
			self.robot.addCmd(self._device, self._cmd, self.__args)
			self._e_stop.wait(self.interval)
			
			
