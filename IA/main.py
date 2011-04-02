# -*- coding: utf-8 -*-


import pince
import robotClient
import eventsWaiter
import loopCmd

## STATES ##
WANT_2_FILL     = 0 # le robot veut se remplir
WANT_2_DUMP     = 1 # le robot veut se vider
FILLING         = 2 # le robot prend un objet
DUMP            = 3 # le robot lache un objet
MOVING          = 4 # le robot est en mouvement
ROTATING        = 5 # le robot tourne
IDENT_OBJ       = 6 # identifier le type d'un pion


class Robot:
    def __init__(self):
        self.pinces = (Pince, Pince)
        self.client = RobotClient()
        self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
        self.pos = (0,0)
        self.target_pos = (-1,-1) # la position du pion visé
        self.events = [] # les events des réponses
        self.reponses = [] # les réponses aux events
        self._e_stop = threading.Event()
    
    def addCmd(self, device, cmd, args=[""]):
        msg = str(device)+C_SEP_SEND+str(cmd)+reduce(lambda (x,y): str(x)+C_SEP_SEND+str(y), params)
        self.events[device][cmd].clear()
        self.client.send(msg)
    
    def write(self, msg):
        self.client.write(msg)
    
    def start(self):
        """ démarage du robot """
        pass
    
    def stop(self, msg=None):
        """ arret du robot """
        if msg: self.write(msg)
        pass
    
    def reset(self):
        """ reset du robot
        self.client, self.pos, self.pinces ne sont pas remis à zéro
        """
        self.stop()
        self.target_pos = (-1,-1) # la position du pion visé
        # clear des events
        for events in self.events:
            for e in events:
                e.clear()
        # clear des pions
        self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
        self._e_stop.clear()
        
    def isEmpty(self):
        return (self.pinces[0].isEmpty() and self.pinces[1].isEmpty())
    
    def isFull(self):
        return (self.pinces[0].isFull() and self.pinces[1].isFull())
    
    def do_path(self, path):
        """ suit un chemin
        La fonction peut tout couper si elle reçoit un message de danger de la tourelle
        ou si après avoir comparé la position actuelle du robot avec celle éstimée 
        il y a une anomalie
        
        @param path la liste des commandes à envoyer à l'asserv sous la forme (cmd, args)
        """
        goals = []
        for cmd,args in path:
            self.addCmd(ASSERV, cmd, args)
            goals.append(args)
        
        
        loopCmd = LoopCmd(self, 1.0, ASSERV, Q_POS)
        
        eventsWaiter = EventsWaiter(("goal", self.events[ASSERV][GOAL_A]), ("pos", self.events[ASSERV][Q_POS]), ("warning", self.events[OTHERS][Q_WARNING]))
        
        while True:
            r = eventsWaiter.wait(1.5)
            if not r: # timeout
                pass
            elif r == "pos":
                if not self.checkAsserv(): # un imprévu est arrivé
                    self.stop("l'asservissement n'est pas là ou il devrait être") 
            elif r == "warning": # danger
                if self.checkWarning(): # danger pour de vrai
                    self.stop("l'asservissement n'est pas là ou il devrait être")
            elif r == "goal": # un but atteind
                goal_reach = self.reponses[ASSERV][GOAL_A]
                if goal_reach in goals: 
                    while True:
                        if goals.pop(0) == goal_reach:
                            break
                    if not goals: # tous les buts sont atteinds
                        break
        
        # arret des threads
        loopCmd.stop()
        eventsWaiter.stop()
        
    
    def takeObject(self, target):
        """
        
        """
        pass
    
    def onEmpty(self):
        """ quand le robot est vide """
        # demande à la camera
        pass
    
    def onFull(self):
        """ quand le robot est plein """
        # aller poser sur des cases
        pass
    
    def waitFor(self, device, cmd, timeout=None):
        e = self.events[device][cmd]
        e.stop_loop = threading.Event()
        if timeout:
            threading.Timer(timeout, lambda: e.stop_loop.set())
        while not self._e_stop.isset() and not e.stop_loop.isset():
            e.wait(1)
            
    def fill(self):
        """ remplir le robot """
        # recherche de la cible
        while True:
            self.addCmd(CAM, Q_SCAN)
            self.waitFor(CAM, Q_SCAN, 1)
            target, path = treatScan()
            if target and path: break
        # envoie du chemin à l'asserv
        for p in path:
            self.send(ASSERV, Q_GOAL_A, p)
        end = path[-1]
        # 
        for p in path:
            self.waitFor(ASSERV, Q_GOAL_A)
            if self.pos == end: break
        
        
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
    
    def onRepCam(self, cmd, msg):
        """ quand la réponse de la camera arrive """
        # il faudra traiter la réponse, voir les pions detectés,
        # leur distance,
        # determiner leur couleur, etc...
        # pour finir aller au but
        if Q_SCAN == cmd:
            self.scan = [ p.split(C_SEP_3) for p in msg.split(C_SEP_2) ]
        self.events[CAM][cmd].set()
    
    def onRepOthers(self, cmd, msg):
        """ quand la carte 'others' envoie un message """
        self.events[OTHERS][cmd].set()
        
    def onRepAsserv(self, cmd, msg):
        """ quand l'asserv envoie un message """
        if Q_POS == cmd:
            self.pos = msg.split(C_SEP2)
        self.events[ASSERV][cmd].set()
    
    def dumpObj(self):
        """ ouvrir la pince """
        self.addCmd(OTHERS,Q_OPEN_PINCE) # ouvre la pince
        self.addCmd(CAM, Q_SCAN) # demande un scan et c'est raparti !
    
    def takeObj(self):
        """ ramasser un objet """
        # demander une identification de l'objet
        # checker si on peut le prendre avec la pince avant ou pince arrière
        # si aucune pince : abandon, demande de scan
        # si pince arrière : rotation
        # avancer vers l'objet
        # fermer la pince
        
    def checkAsserv(self, new_pos):
        """ check si l'asserv s'en sort bien 
        @todo coder la fonction si necessaire
        """
        pass
    
    def treatScan(self):
        """ traitement du scan de la carte avec notre position actuelle 
        @return target,path
        """
        # algo de détermination quel pion proche (probablement le plus proche)
        # pathfinding pour esquiver nos pions
        # retourne la cible choisie et le path
        # self.client.send("asserv", Q_GOAL_A, <devant le pion>)
        # self.client.Send("asserv", Q_ANGLE_A, <faceau pion>)
        pass
    
    def onWarning(self, mag):
        """ quand il y a danger """
        # definir une stratégie de coutournement
        # OU
        # nouveau scan, mais seulement derière
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
    