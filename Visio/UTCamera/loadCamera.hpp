#ifndef _loadCamera
#define _loadCamera
/**
 * \file loadCamera.hpp
 * \brief Algorithmes de reconnaissance des caméras
 * \version 1
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
// System
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// OpenCV
#include <cv.h>
#include <highgui.h>

// Extern
#include "constantes.hpp"

    /*** EXTERN GLOBAL ***/

extern CvCapture*      captureCameraF;
extern CvCapture*      captureCameraB;

    /*** PROTOTYPE ***/

void identificationCamera();
void checkIndexValide(int *index1, int *index2, int *nb);
void loadUsbVision(int* usb1, char* serial1, int* usb2, char* serial2);

// --- loadCamera ---
#endif
