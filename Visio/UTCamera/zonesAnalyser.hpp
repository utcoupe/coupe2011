#ifndef zonesAnalyser_H
#define zonesAnalyser_H
    /*** INCLUDE ***/
// System
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// OpenCV
#include <cv.h>
#include <highgui.h>

// Extern
#include "constantes.hpp"
#include "regionLister.hpp"


    /*** DEFINES ***/


    /*** EXTERN GLOBAL ***/
extern regionLister*   listBeforeTrait;
extern regionLister*   listFinal;

extern IplImage*       imgBas;
extern IplImage*       imgBin;

    /*** PROTOTYPE ***/
void regionGrowing();
void afficheListeRegion(regionLister* rl, IplImage* imgPtr);
regionLister* analyseListeRegion(regionLister* listeAanalyser, int width, int height);

double getLargeurTheoriqueDuPiont(int yDuBasDuPiont);
double getLengthRobotToRegion(double yDuBasDuPiont);

#endif
