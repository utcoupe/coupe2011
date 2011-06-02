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


print os.path.join(ROOT_DIR,"Visio","Smartphone","bin")
# ===> usage: noview/view ip_serveur_python ip_smartphone
p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main","noview", "10.42.43.94", "10.42.43.10"]);
	

p.wait()


