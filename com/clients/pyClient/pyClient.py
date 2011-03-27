# -*- coding: utf-8 -*-

import socket
import threading

class pyClient:
    """ @author Thomas
        
    classe python de base pour communiquer avec le serveur
    """
    
    def __init__(self, host, port, fn_input, fn_on_close=None):
        """
        @param host ip dus erveur
        @param port port sur lequel écouter
        @param fn_input fonction qui doit renvoyer les données à envoyer
        """
        self._fn_input = fn_input
        self._fn_on_close = fn_on_close
        self._server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._server.connect((host, port))
    
    def start(self):
        self.t_send = threading.Thread(None, self._fn_send, None)
        self.t_recv = threading.Thread(None, self._fn_recv, None)
        self.t_send.start()
        self.t_recv.start()

    def _fn_send(self):
        """ 
        Envoie le message récupéré par self._fn_input() au serveur
        """
        while True:
            msg = self._fn_input()
            print "Send :",msg
            self._server.send(msg)
            if msg == 'close' or msg == 'shutdown':
                print "break send"
                break
    
    def _fn_recv(self):
        """ 
        Récupère ce qu'envoie le serveur
        """
        while True:
            data = self._server.recv(1024) # en octets/chars
            if not data or str(data) == 'close':
                self._server.close()
                if self._fn_on_close : self._fn_on_close()
                print "break recv"
                break
            print 'Received :', repr(data)

