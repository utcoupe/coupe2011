#!/bin/bash

gnome-terminal -e "./smartphone.py a"
echo "press"
read
gnome-terminal -e com/serverPython/main.py
sleep 1
gnome-terminal -e IA/main.py
gnome-terminal -e ./smartphone.py
