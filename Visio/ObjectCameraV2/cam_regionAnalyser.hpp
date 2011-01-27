/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    cam_regionAnalyser.cpp
    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet d'analyser une liste de region:
*       - Détermine si la zone se compose de n sous
*         zones
*       - Détermine le type de la zone (piont...)
*       - Détermine les distances en mm qui séparent
*         le robot de l'objectif en x et y relatif.
*****************************************************
 **/
/**       NORMES DANS CE FICHIER                   **
*****************************************************
Distance en mm
x' et y' coordonnées relatives aux robots
                  x'
              |---------O obejctif
           y' |
              |
            ----
            |  | robot
            ----
*****************************************************
**/
#ifndef _regionAnalyser
#define _regionAnalyser
/****************************************************
*****           INCLUDES                        *****
****************************************************/
// System
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// OpenCV
#include <cv.h>
using namespace cv;
#include <highgui.h>

// Extern
#include "cam_regionLister.hpp"
#include "cam_utility.hpp"
/****************************************************
*****           MACRO-DEFINITION                *****
****************************************************/

// Definitions
#define DIAMETRE_PIONT 200.0
#define RAYON_PIONT    100.0
#define MAX_REGION     30
/****************************************************
*****           PROTOTYPES                      *****
****************************************************/
double getLargeurTheoriqueDuPiont(int yDuBasDuPiont);
double getLengthRobotToRegion(int yDuBasDuPiont);

regionLister* analyseListeRegion(regionLister* listeAanalyser, int width, int height);
void afficheListeRegion(regionLister* rl, IplImage* imgPtr);

void fenetreDeParametrageDesCourbes(int camera);

/****************************************************
*****           REGION ANALYSER                 *****
****************************************************/
class regionAnalyser
{
    public:

        regionAnalyser();

        void analyse                   (regionLister listDeRegion);
        regionLister getListeResultats (){return listeResultats;};

        ~regionAnalyser();

    private:

        regionLister    listeResultats;

};

#endif // --- _regionAnalyser ---
