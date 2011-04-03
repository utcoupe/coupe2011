# -*- coding: utf-8 -*-



class Vec2:
    def __init__(self,x,y):
        self.x = int(round(x))
        self.y = int(round(y))
    
    def tracer(self, canevas, color="black"):
        return canevas.create_oval(self.x-3,self.y-3,self.x+3,self.y+3, fill=color, width=3)
    
    def __repr__(self):
        return "Vec2"+str((int(self.x),int(self.y)))
    
    def __add__(self,other):
        return Vec2(self.x+other.x, self.y+other.y)
    