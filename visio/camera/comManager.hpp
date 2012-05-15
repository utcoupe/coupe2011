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
#include "Fps.hpp"
#include "regionLister.hpp"
#include "colorConvertion.hpp"
#include "zonesAnalyser.hpp"

using namespace std;

    /*** EXTERN GLOBAL ***/

extern CvCapture*      captureCameraF;
extern CvCapture*      captureCameraB;


    /*** PROTOTYPE ***/

void modeStationnaire();
void* grabCamF(void* data);
void* grabCamB(void* data);

