#!/bin/bash
#Script pour Obtenir l'iSerial des cameras avec leurs ports usb correspondant 
#Format  portUsb iSerial

motDePasse="tomtom"
pathGlobal="CamFiles/"
portUsb=$(lsusb | grep 046d:0807 | cut -c 5-7,15-18 | sed -e "s/ /:/g")	
nbDeCam=$(echo $portUsb | wc -w)

#Si il n'y a qu'une webcam on se prend pas la tête
if [ $nbDeCam -eq 1 ]
then	
	ser1=$( echo $motDePasse | sudo -S lsusb -v -s $portUsb | grep iSerial | cut -c 29-40 )
	portUsb=$(echo $portUsb | cut -c 5-7 ) 
	echo $portUsb $ser1 > $pathGlobal"usbVision"
fi

#Si il y a en deux on divise 
if [ $nbDeCam -eq 2 ]
then	
	portUsb1=$(echo $portUsb | cut -c 1-7)
	portUsb2=$(echo $portUsb | cut -c 8-15)

	ser1=$( echo $motDePasse | sudo -S lsusb -v -s $portUsb1 | grep iSerial | cut -c 29-40 )
	portUsb1=$(echo $portUsb1 | cut -c 5-7 ) 
	echo $portUsb1 $ser1 > $pathGlobal"usbVision"

	ser2=$( echo $motDePasse | sudo -S lsusb -v -s $portUsb2 | grep iSerial | cut -c 29-40 )
	portUsb2=$(echo $portUsb2 | cut -c 5-7 ) 
	echo $portUsb2 $ser2 >> $pathGlobal"usbVision"

fi
