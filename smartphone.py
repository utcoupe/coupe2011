#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
ROOT_DIR  = os.path.dirname(os.path.abspath(__file__))

import sys
import subprocess

#if len(sys.argv) > 1:
#	p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main"])
#else:
#	p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main","noview"])

view = "noview"
if len(sys.argv) > 1:
	view = "view"

print view
print os.path.join(ROOT_DIR,"Visio","Smartphone","bin")
# ===> usage: noview/view ip_serveur_python ip_smartphone
p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main",view, "utcoupe-AOD255.local", "192.168.43.1"]);

	

p.wait()


