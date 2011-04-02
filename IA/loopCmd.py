# -*- coding: utf-8 -*-


import threading

class LoopCmd(threading.Thread):
    def __init__(self, robot, interval, device, cmd, *args):
        threading.Thread.__init__(None, None, "LoopCmd(%s,%s,%s,%s)"%(interval,device,cmd,args))
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
            
            
