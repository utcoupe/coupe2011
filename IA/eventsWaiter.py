# -*- coding: utf-8 -*-

import threading


class EventsWaiter:
    """
    Permet de wait plusieurs events en même temps
    """
    def __init__(self, events2listen):
        """
        @param events2listen liste des events à écouter, sous la forme (name, event)
        """
        self._e_stop = threading.Event()
        self.events = events2listen
        
    def __del__(self):
        self.stop()
        
    def stop(self):
        self._e_stop.set()
    
    def wait(self, timeout=None):
        """
        @param timeout timeout
        @return la nom de l'event ou None
        """
        self._wait = timeout and min(timeout,1.0) or 1.0
        if timeout:
            threading.Timer(timeout, self.stop)
        
        for name,e in self.events:
            threading.Thread(None, self._loop_listener, "EventWaiter->loop(%s)"%name, (name,e,))
        
        self._e_stop.wait(timeout)
        
        for name,e in self.events:
            if e.set(): return name
        
        
        
    def _loop_listener(self, name, e):
        while not self._e_stop.is_set():
            e.wait(self._wait)
            if e.is_set(): self._e_stop.set()
    
    
    