#include <iostream>

#include <cv.h>
#include <highgui.h>

#include "UTCamera.hpp"

#define NOM_FENETRE "CamCaptor"

using namespace std;


int main()
{

    UTCam *camAv = new UTCam(CAMERA_AVA);


    camAv->calibrage();



    return 0;
}
