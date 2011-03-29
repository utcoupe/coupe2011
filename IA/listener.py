# -*- coding: utf-8 -*-

import sys
sys.path.append('../com/clients/pyClient')
import pyClient

class Listener:
    def __init__(self, robot):
        host = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
        port = sys.argv[2] if len(sys.argv) > 2 else 50000            # The same port as used by the server
        self.client = pyClient.pyClient(host, port, self._fn_output, self._fn_input)
        self.client.start()
        self.robot = robot
    
    def _fn_output(self):
        """ fonction appellée quand un message est reçu """
        # formatage de la réponse et appelle d'un robot.on<Event>()
        pass
    
    def _fn_input(self):
        """ fonction appellée pour envoyer un message """
        # lecture de messages du robot
        return

    