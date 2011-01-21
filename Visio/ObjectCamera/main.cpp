#include <iostream>

#include "cv.h"
#include "highgui.h"

#include "UTCamera.hpp"

#define NOM_FENETRE "CamCaptor"

using namespace std;


int main()
{

    //UTCamera *camAv = new UTCamera(CAMERA_AVA, 1280, 800);
    UTCamera *camAv = new UTCamera(CAMERA_ARR, 1280, 800);

    camAv->calibrage();


    return 0;
}
