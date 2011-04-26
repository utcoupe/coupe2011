/**
 * \file camCalibre.hpp
 * \brief Algorithmes pour calibrer les caméras
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

// OpenCV
#include <cv.h>
#include <highgui.h>

// Extern
#include "constantes.hpp"
#include "colorConvertion.hpp"
#include "Fps.hpp"

using namespace std;

    /*** EXTERN GLOBAL ***/

extern CvCapture*      captureCameraF;
extern CvCapture*      captureCameraB;

extern IplImage*       imgBas;
extern IplImage*       imgBin;
extern IplImage*       imgBin2;

    /*** PROTOTYPE ***/

void calibrationManuel();
void saveConfig();
void loadConfig();
void souris(int event, int x, int y, int flags, void *param);
