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


# ===> usage: noview/view ip_serveur_python ip_smartphone
p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main","noview", "10.42.43.1", "192.168.43.1"]);
	

p.wait()


