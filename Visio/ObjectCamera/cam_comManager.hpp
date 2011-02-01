/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    UTCoupe 2010-2011
    All rights reserved.

*   Gére la communication avec l'intelligence
*****************************************************
 **/
#ifndef _comManager
#define _comManager
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
// ----------------------------------------
Format des ordres
cin >> numéro caméra >> " "

numéra caméra == entier
1 -> caméra avant
2 -> caméra arriére
// ------------------------------------------
Format des trames de retour si un piont a été trouvé
cout << type << " " << x' << " " << y' << ",";

type 1 char ->  'p' pour piont
                'f' pour figure
                't' pour tour

Si aucun piont
cout << 'r';
// ------------------------------------------

*****************************************************
**/
/****************************************************
*****           PROTOTYPES                      *****
****************************************************/
char waitOrder();
void send(regionLister* rl);


#endif
