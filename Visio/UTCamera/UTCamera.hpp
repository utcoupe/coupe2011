#ifndef _UTCamera
#define _UTCamera
/**
 * \file UTCamera.cpp
 * \brief fichier principale du projet vision
 * \version 1
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
// System
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <list>

// OpenCV
#include <cv.h>
#include <highgui.h>

// Extern
#include "constantes.hpp"
#include "regionLister.hpp"
#include "loadCamera.hpp"
#include "comManager.hpp"
#include "camCalibre.hpp"
#include "zonesAnalyser.hpp"

using namespace std;

    /*** GLOBAL ***/

// Caméra du robot
CvCapture*      captureCameraF;
CvCapture*      captureCameraB;

IplImage*       imgBas;
IplImage*       imgBin;
IplImage*       imgHsv;
IplImage*       imgBin2;

int             _H[NB_COLOR];
int             _Htol[NB_COLOR];
int             _S[NB_COLOR];
int             _Stol[NB_COLOR];

regionLister*   listBeforeTrait;
regionLister*   listFinal;

pthread_t       th[NB_THREAD];

    /*** PROTOTYPE ***/

void initGlobalVariable();

// --- UTCamera ---
#endif
