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
#ifndef _UTILCAM
#define _UTILCAM
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
*****           PROTOTYPES                      *****
****************************************************/
string intToString(int entier);
int chareToInt(char* car);


#endif
