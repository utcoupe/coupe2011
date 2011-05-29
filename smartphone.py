#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
ROOT_DIR  = os.path.dirname(os.path.abspath(__file__))

import sys
import subprocess

if len(sys.argv) > 1:
	p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main"])
else:
	p = subprocess.Popen(["java","-cp",os.path.join(ROOT_DIR,"Visio","Smartphone","bin"),"Main","noview"])
	

p.wait()

p.terminate()
p.kill()
