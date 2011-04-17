# -*- coding: utf-8 -*-

import threading
from Queue import Queue, Empty

class EventsWaiter:
    """
    Permet de wait plusieurs events en même temps
    """
    def __init__(self, *events2listen):
        """
        @param events2listen liste des events à écouter, sous la forme (name, event)
        """
        self._e_stop = threading.Event()
        self.events = events2listen
        self._queue = Queue
        
        for name,e in self.events:
            threading.Thread(None, self._loop_listener, "EventWaiter->loop(%s)"%name, (name,e,))
        
    def __del__(self):
        self.stop()
    
    def stop(self):
        self._e_stop.set()
    
    def wait(self, timeout=None):
        """
        @param timeout timeout
        @return la nom de l'event ou None
        """
        try:
            r =self._queue.get(true, timeout)
        except Empty:
            return None
        else:
            return r
        
    def _loop_listener(self, name, e):
        while not self._e_stop.is_set():
            e.wait(1.0)
            if e.is_set():
                e.clear()
                self._queue.put(name)
    
    
    