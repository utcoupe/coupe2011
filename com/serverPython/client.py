# -*- coding: utf-8 -*-

import threading
import traceback

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
        self.id = id
        self._server = server
    
    def send(self, msg):
        """
        @param msg message à envoyer
        """
        self.s.send(msg)
        
    def run(self):
        """
        Point d'entrée, envoie au client son id puis lance self._loop() en boucle
        """
        self.send(str(self.id))
        while not self._server.e_shutdown.isSet():
            self._loop()
            self._server.e_shutdown.wait(0.0001)
        self.s.close()
        print "Client(%s) arreté"%self.id
        
    def _loop(self):
        """
        Fonction appellée en boucle
        récupère le message et le traite
        """
        msg = self.s.recv(1024)
        if msg:
            print "Received from", self.id,":",msg
            msg_split = msg.strip().split(" ",1)
            try:
                if 'test' == msg_split[0]:
                    id_device,cmd = msg_split[1].split(' ',1)
                    self._server.testPing(id_device, cmd)
                elif 'shutdown' == msg_split[0]:
                    self._server.e_shutdown.set()
                    self._server.stop()
                    self.send('close')
                elif 'close' == msg_split[0]:
                    self._server.closeClient(self.id)
                    self.send('close')
                elif 'loop' == msg_split[0]:
                    msg_split2 = raw_input().split(' ',1)
                    self._server.makeLoop(msg_split2[0], msg_split2[1], msg_split[1])
                elif 'stop' == msg_split[0]:
                    self._server.stopScreen(int(msg_split[1]))
                else:
                    print msg_split
                    self._server.addCmd(msg_split[1], msg_split[0])
            except IndexError as ex:
                print "mauvaise commande, IndexError : %s"%ex
                traceback.print_exc()
                self.send("mauvaise commande, IndexError : %s"%ex)
            except KeyError as ex:
                print "mauvaise commande, KeyError : %s"%ex
                traceback.print_exc()
                self.send("mauvaise commande, KeyError : %s"%ex)
            