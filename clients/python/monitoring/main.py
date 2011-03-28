# -*- coding: utf-8 -*-


import sys
sys.path.append('../../../com/clients/pyClient')
import pyClient


HOST = sys.argv[1] if len(sys.argv) > 1 else 'localhost'
PORT = sys.argv[2] if len(sys.argv) > 2 else 50000            # The same port as used by the server

client = pyClient.pyClient(HOST, PORT, raw_input)
client.start()

