# -*- coding: utf-8 -*-

import threading
import socket
import sys
sys.path.append('../com/serverPython/')
from protocole import *


        
class RobotClient(threading.Thread):
    def __init__(self, robot):
        threading.Thread.__init__(self,None,None,"RobotClient")
        self.robot = robot
        self._lock_write = threading.Lock()
        self._e_close = threading.Event()
        
        host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
        port = int(sys.argv[2]) if len(sys.argv) > 2 else 50000            # The same port as used by the server
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._socket.settimeout(1.0)
        self._socket.connect((host, port))

    def send(self, msg):
        """ 
        Envoie le message au serveur
        """
        if not self._e_close.isSet():
            self._socket.send(msg)
            self.write("Send : %s"%msg)
            if msg == 'close':
                self._e_close.set()
                self.write("break send")
    
    def run(self):
        """ 
        Récupère ce qu'envoie le serveur
        """
        while not self._e_close.isSet():
            try:
                data = self._socket.recv(1024) # en octets/chars
            except socket.timeout:
                pass
            else:
                if not data or str(data) == 'close':
                    self._socket.close()
                    self._e_close.set()
                    self._write("break recv")
                self._treat(data)
                
    def _treat(self, msg):
        """ fonction appellée quand un message est reçu """
        # formatage de la réponse et appelle d'un robot.on<Event>()
        self.write("Received : %s"%msg)
        msg = str(msg)
        id_from, id_msg, msg = msg.split(C_SEP_SEND,2)
        id_from = int(id_from)
        id_msg = int(id_msg)
        if id_from == ID_SERVER:
            self.robot.onRepServer(id_msg, msg)
        elif id_from == ID_ASSERV:
            self.robot.onRepAsserv(id_msg, msg)

    def write(self, msg):
        """ pour écrire sans se marcher sur les doigts """
        self._lock_write.acquire()
        print str(msg).strip()
        self._lock_write.release()
    