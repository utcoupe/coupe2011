#!/bin/bash

gnome-terminal -e ./smartphone.py
echo "press"
read
gnome-terminal -e com/serverPython/main.py
sleep 1
gnome-terminal -e IA/main.py

