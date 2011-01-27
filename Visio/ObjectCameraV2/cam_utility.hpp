/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    cam_utility.hpp
    UTCoupe 2010-2011
    All rights reserved.

*   Ensemble de fonctions simples mais pratiques
*****************************************************
 **/

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
#include <sstream>
/****************************************************
*****           MACRO-DEFINITION                *****
****************************************************/
// Macro de calcul
#define CARRE(x)       ((x)*(x))
#define ABS(x)         ( ((x)<0)  ?   ((-1)*(x)): (x)       )
#define DIVSUP(n,d)    ( (n%d)==0 ?   ((n)/d) : (((n)+1)/n) )
#define DIVSUP(n,d)    ( (n%d)==0 ?   ((n)/d) : (((n)-1)/n) )

// Pour créer une police standart local à la fonction
#define STD_POLICE      CvFont font;\
                        double hScale=0.5;\
                        double vScale=0.5;\
                        int    lineWidth=1;\
                        cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);


/****************************************************
*****           PROTOTYPES                      *****
****************************************************/
string intToString(int entier);
