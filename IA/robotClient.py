# -*- coding: utf-8 -*-

import sys
sys.path.append('../com/serverPython/')
import protocol


        
class RobotClient:
    def __init__(self, robot):
        self.robot = robot
        self._lock_write = threading.Lock()
        self._e_close = threading.Event()
        
        host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
        port = sys.argv[2] if len(sys.argv) > 2 else 50000            # The same port as used by the server
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._socket.settimeout(1.0)
        self._socket.connect((host, port))
        
    def start(self):
        self.t_recv = threading.Thread(None, self._fn_recv, None)
        self.t_recv.start()
        self.write("Standby ready !")

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
    
    def _fn_recv(self):
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
        self.write("Received : %s"%data)
        msg = str(msg)
        device, cmd, rep = 0,int(0),0
        if device == "asserv":
            self.robot.onRepAsserv(cmd, rep)
        elif device == "cam":
            self.robot.onRepCam(cmd, rep)

    def write(self, msg):
        """ pour écrire sans se marcher sur les doigts """
        self._lock_write.acquire()
        print msg.strip()
        self._lock_write.release()
    