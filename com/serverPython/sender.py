# -*- coding: utf-8 -*-

import threading
from Queue import Queue, Empty

class Sender(threading.Thread):
    def __init__(self, server):
        threading.Thread.__init__(self,None, None, "Sender")
        self._server = server
        self._queue = Queue()
    
    def addMsg(self, mask_from, to, msg):
        self._queue.put((mask_from,to,msg))
    
    def run(self):
        while not self._server.e_shutdown.is_set():
            try:
                mask_from, to, msg = self._queue.get(True, 2)
            except Empty:
                pass
            else:
                self._send(mask_from, to, msg)
                self._queue.task_done()
    
    def _send(self, mask_from, to, msg):
        for c in self._server.clients:
            if to & (1 << c.id):
                c.send(mask_from, msg)
        
        
        
        
        
        
        
        
        
        
        
        
        