# -*- coding: utf-8 -*-



class Sender:
    def __init__(self, clients):
        self.clients = clients
    
    def send(self, mask_from, to, msg):
        for c in self.clients:
            if to & (1 << c.id):
                c.send(mask_from, msg)
        
        