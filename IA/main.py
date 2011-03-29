# -*- coding: utf-8 -*-


import pince
import listener



class Robot:
    def __init__(self):
        self.pinces = (Pince, Pince)
        self.listener = Listener()
        self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
    
    def start(self):
        """ démarage du robot """
        pass
    
    def stop(self):
        """ arret du robot """
        pass
    
    def reset(self):
        """ reset du robot """
        self.__init__()
    
    def isEmpty(self):
        return (self.pinces[0].isEmpty() and self.pinces[1].isEmpty())
    
    def isFull(self):
        return (self.pinces[0].isFull() and self.pinces[1].isFull())
        
    def onEmpty(self):
        """ quand le robot est vide """
        # demande à la camera
        pass
    
    def onFull(self):
        """ quand le robot est plein """
        # aller poser sur des cases
        pass
    
    def onDetectEnemy(self):
        """ quand l'adversaire est trop proche """
        # déterminer qui est en tord
        # si c'est nous, alors demarche d'esquive
        pass
    
    def onLostEnemy(self):
        """ lorsque la detection de l'adversaire est perdue """
        # ralentir la vitesse pour etre sur de ne pas lui foncer dedans
        pass
    
    def onReconnectEnnemy(self):
        """ lorsque l'on redetect l'adversaire """
        # accellerer la vitesse
        pass
    
    def onDetectPion(self):
        """ quand les sharps detecte un pion 
        (arrive lorsque l'on est suffisament proche pour le prendre) """
        # il faut voir ici à quoi on a à faire,
        # pion, double pion, tour, pion+tour, ...
        # ensuite il faut appeller la fonction canAccept des pinces
        # pour voir si on peut prendre ce pion
        # si on peut le prendre, alors on lance la fonction de ramassage
        # sinon on repart sur une demande à la camera
        pass
    
    def onReponseCam(self):
        """ quand la réponse de la camera arrive """
        # il faudra traiter la réponse, voir les pions detectés,
        # leur distance,
        # determiner leur couleur, etc...
        # pour finir aller au but
        pass
    
    def onGoToPos(self):
        """ c'est bon on a une position à atteindre """
        pass
    
    def onStop(self):
        """ quand le robot doit s'arreter """
        self.stop()
    
    def onStart(self):
        """ quand le robot doit demarrer """
        self.start()
    
    def onReset(self):
        """ quand le robot doit se reseter """
        self.reset()
    
if __name__ == '__main__':
    robot = Robot()    
    robot.start()
    