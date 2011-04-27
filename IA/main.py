# -*- coding: utf-8 -*-


from pince import *
from robotClient import *
from msgFifo import *
from loopCmd import *



MAX_MSG         = 50


class Robot:
    def __init__(self):
        """
        Lorsque le robot envoie un message, il envoi aussi un id_msg compris entre 0 et MAX_MSG
        Lorsqu'il reçoit un message, il sait de qui il provient grâce au server,
        il retrouve ensuite à quelle commande il correspond grâce à l'id_msg,
        il active alors deux events : celui self.events[id_device][id_cmd] et celui
        self.msg_events[id_msg]
        """
        self.pinces = (Pince, Pince)
        self.client = RobotClient(self)
        
        self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
        self.pos = (0,0)
        self.target_pos = (-1,-1) # la position du pion visé
        
        self._e_stop = threading.Event()
        
        self.cmd = [0] * MAX_MSG # self.cmd[id_msg] = id_cmd
        self.id_msg = 0
    
    
    def addCmd(self, id_device, id_cmd, args=[""]):
        """
        Envoyer une commande
        
        @param id_device id du device (asserv, cam,...)
        @param id_cmd id de la commande
        @param args les éventuels arguments
        
        @return id_msg
        """
        # création du message
        msg = str(id_device)
        if id_device != ID_CAM:
			msg += C_SEP_SEND+str(self.id_msg)
        msg = C_SEP_SEND+str(id_cmd)
        for a in args: msg += C_SEP_SEND+str(a)
        
        # sauvegarde de la commande
        self.cmd[self.id_msg] = id_cmd
        
        # envoie
        self.client.send(msg)
        
        # increment de id_msg
        self.id_msg += 1
        if self.id_msg >= MAX_MSG:
            self.id_msg = 0
        
        # return de id_msg
        return 0 if self.id_msg==0 else (self.id_msg-1)
    
    def write(self, msg):
        self.client.write(msg)
    
    def start(self):
        """ démarage du robot """
        self.client.start()
        
        fifo = self.client.addFifo( MsgFifo(((ID_ASSERV, Q_POSITION),)) )
        
        self.addCmd(ID_ASSERV, Q_POSITION)
        r = fifo.getMsg()
        self.write(r)
        self.pos = r
        
        """self.addCmd(ID_ASSERV, Q_AUTO_CALIB, (0,))
        self.events[ID_ASSERV][Q_AUTO_CALIB].wait()
        self.events[ID_ASSERV][Q_AUTO_CALIB].wait()
        self.write(self.reponses[ID_ASSERV][Q_AUTO_CALIB])
        """
        vitesse = 180
        self.do_path((Q_GOAL_ABS, (1000,0,vitesse)),
                     (Q_ANGLE_ABS, (90,vitesse-80)),
                     (Q_GOAL_ABS, (1000,1000,vitesse)),
                     (Q_ANGLE_ABS, (180,vitesse-80)),
                     (Q_GOAL_ABS, (0,1000,vitesse)),
                     (Q_ANGLE_ABS, (-90,vitesse-80)),
                     (Q_GOAL_ABS, (0,0,vitesse)),
                     (Q_ANGLE_ABS, (0,vitesse-80)),
                    )
        
        self.client.removeFifo(fifo)
    
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
        self.id_msg = 0
        # clear des pions
        self.pions = [] # la liste des pions que l'on a déjà vu pour pouvoir faire des estimations par la suite
        self._e_stop.clear()
        
    def isEmpty(self):
        return (self.pinces[0].isEmpty() and self.pinces[1].isEmpty())
    
    def isFull(self):
        return (self.pinces[0].isFull() and self.pinces[1].isFull())
    
    def do_path(self, *path):
        """ suit un chemin
        La fonction peut tout couper si elle reçoit un message de danger de la tourelle
        ou si après avoir comparé la position actuelle du robot avec celle éstimée 
        il y a une anomalie
        
        @param path la liste des commandes à envoyer à l'asserv sous la forme (cmd, args)
        """
        goals = []
        for cmd,args in path:
            self.addCmd(ID_ASSERV, cmd, args)
            goals.append(args)
        
        
        loopCmd = LoopCmd(self, 1.0, ID_ASSERV, Q_POSITION)
        
        #eventsWaiter = EventsWaiter(("goal", self.events[ID_ASSERV][Q_GOAL_ABS]), ("pos", self.events[ID_ASSERV][Q_POSITION]), ("warning", self.events[ID_OTHERS][Q_WARNING]))
        """
        @todo
        while True:
            r = eventsWaiter.wait(1.5)
            if not r: # timeout
                pass
            elif r == "pos":
                if not self.checkAsserv(): # un imprévu est arrivé
                    self.stop("l'asservissement n'est pas là ou il devrait être")
                self.write(self.reponses[ID_ASSERV][Q_POSITION])
            elif r == "warning": # danger
                if self.checkWarning(): # danger pour de vrai
                    self.stop("l'asservissement n'est pas là ou il devrait être")
            elif r == "goal": # un but atteind
                self.write(self.reponses[ID_ASSERV][Q_GOAL_ABS])
                goal_reach = self.reponses[ID_ASSERV][Q_GOAL_ABS]
                if goal_reach in goals: 
                    while True:
                        if goals.pop(0) == goal_reach:
                            break
                    if not goals: # tous les buts sont atteinds
                        break"""
        
        # arret des threads
        loopCmd.stop()
        eventsWaiter.stop()
        
    def scan(self):
        """
        demande un scan à la camera
        """
        addCmd(ID_CAM, Q_SCAN_AV)
    
    
    
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
    
    def dumpObj(self):
        """ ouvrir la pince """
        self.addCmd(ID_OTHERS,Q_OPEN_PINCE) # ouvre la pince
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
        # self.client.send("asserv", Q_GOAL_ABS, <devant le pion>)
        # self.client.Send("asserv", Q_ANGLE_ABS, <faceau pion>)
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
    
