# -*- coding: utf-8 -*-

import threading
import socket

from protocole import *

class Client(threading.Thread):
    def __init__(self, server, id, name):
        threading.Thread.__init__(self, None, None, name)
        self._server = server # la socket pour envoyer recevoir
        self.id = id # id du client sur le serveur
        self._running = False # le client tourne
        self.mask_recv_from = -1 # ALL
    
    def __del__(self):
        self.s.close()
        print "%s destroy"%self.name
        
    def stop(self):
        self._running = False
    
    def send(self, mask_from, msg):
        """
        @param mask_from id du client qui a envoyé le message
        @param msg message à envoyer
        """
        if self.mask_recv_from & mask_from:
            self._fn_send(msg)
    
    def run(self):
        """
        Point d'entrée, envoie au client son id puis lance self._loop() en boucle
        """
        print "%s start"%self.name
        self._running = True
        self.send(-1,str(self.id))
        while self._running and not self._server.e_shutdown.isSet():
            self._loopRecv()
        print "%s arreté"%self.name
    
    
class TCPClient(Client):
    """
    Client TCP
    """
    def __init__(self, server, id, s):
        """
        @param server le server
        @param id id du client
        @param s socket pour écouter envoyer
        """
        Client.__init__(self, server, id, "TCPClient(%s)"%id)
        self.s = s
        self.s.settimeout(1.0) # timeout
    
    def _fn_send(self, msg):
        self.s.send(msg)
        
    def _loopRecv(self):
        try:
            msg = self.s.recv(1024)
        except socket.timeout:
            pass
        else:
            self._server.write("Received from %s : %s"%(self.name,msg))
            if msg:
                self._server.parseMsg(self.id, msg)
            else:
                self._server.parseMsg(self.id, "close")


class LocalClient(Client):
    """
    Le client qui est dans le terminal lancé par main.py
    """
    def __init__(self, server, id):
        Client.__init__(self, server, id, "LocalClient(%s)"%id)
    
    def _fn_send(self, msg):
        self._server.write("Received : %s"%(msg))
    
    def _loopRecv(self):
        msg = raw_input()
        if msg:
            self._server.parseMsg(self.id, msg)
        else:
            self._server.parseMsg(self.id, "close")
        
class SerialClient(Client):
    """
    connection aux cartes arduinos
    """
    def __init__(self, server, id, serial, port, baudrate):
        Client.__init__(self, server, id, "SerialClient(%s)"%id)
        self.serial = serial
        self.port = port
        self.baudrate = baudrate
    
    def _fn_send(self, msg):
        self.serial.write(str(msg)+"\n")
        
    def _loopRecv(self):
        msg = self.serial.readline()
        if msg:
            self._server.write("Received from %s : %s"%(self.name,msg))
            self._server.parseMsg(self.id, msg)

    def stop(self):
        self.serial.close()
        Client.stop(self)

class SubprocessClient(Client):
    def __init__(self, server, id, process, exec_name):
        Client.__init__(self, server, id, "SubprocessClient(%s)"%s)
        self.process = process
        self.exec_name = exec_name
        
    def _fn_send(self, msg):
        self.process.stdin.write(str(msg)+"\n") # envoie au child
        self.process.stdin.flush()
    
    def _loopRecv(self):
        msg = self.process.stdout.readline()
    
    def stop(self):
        self.process.kill()
        Client.stop(self)


