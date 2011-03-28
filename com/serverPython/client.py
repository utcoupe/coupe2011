# -*- coding: utf-8 -*-

import threading
import traceback
import socket

from protocole import *


class Client(threading.Thread):
    """
    Client
    """
    def __init__(self, id, s, server):
        """
        @param id id du client
        @param s socket pour écouter envoyer
        @param server le server
        """
        threading.Thread.__init__(self, None, None, "Client(%s)"%str(id))
        self.s = s
        self.s.settimeout(1.0)
        self.id = id
        self._server = server
        self._running = True
        
    def send(self, msg):
        """
        @param msg message à envoyer
        """
        self.s.send(msg)
    
    def __del__(self):
        self.s.close()
        print "Client(%s) destroy"%self.id
        
    def run(self):
        """
        Point d'entrée, envoie au client son id puis lance self._loop() en boucle
        """
        print "Client(%s) start"%self.id
        self.send(str(self.id))
        while self._running and not self._server.e_shutdown.isSet():
            self._loop()
        print "Client(%s) arreté"%self.id
        
    def _loop(self):
        """
        Fonction appellée en boucle
        récupère le message et le traite
        """
        try:
            msg = self.s.recv(1024)
        except socket.timeout:
            pass
        else:
            if msg:
                print "Received from", self.id,":",msg
                self._server.parseMsg(msg)
            