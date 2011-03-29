# -*- coding: utf-8 -*-

VIDE            = 0
PION_1          = 1 # pion simple
PION_2          = 2 # double pion
PION_1_T        = 3 # pion simple + tour
PION_2_T        = 4 # pion double + tour
TOUR            = 5 # tour simple

ACCEPT = {}
ACCEPT[VIDE]        = (PION_2_T, TOUR, PION_1_T, PION_2, PION_1)
ACCEPT[PION_1]      = (PION_1)
ACCEPT[PION_2]      = ()
ACCEPT[PION_1_T]    = (PION_1)
ACCEPT[PION_2_T]    = ()
ACCEPT[TOUR]        = (PION_2, PION_1)



class Pince:
    """ Class représentant une pince """
    def __init__(self):
        self.objet = VIDE # l'objet que tiens les pinces
        pass
    
    def canAccept(self, object):
        """
        Si on peut encore prendre un objet (flag représentant 
        les diff configurations de pions)
        """
        return (objet in ACCEPT[self.objet])
    
    def listCanAccept(self):
        """ Retourne la liste des objets (flag) que la pince peut accepter) """
        return ACCEPT[self.objet]
    
    def isEmpty(self):
        """ si la pince est vide """
        return (self.objet == VIDE)
    
    def isFull(self):
        """ si la pince est pleine """
        return (not self.listAccept())
        
        
        