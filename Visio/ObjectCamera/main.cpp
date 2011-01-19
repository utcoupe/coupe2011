#include <iostream>

#include "cv.h"
#include "highgui.h"

#include "UTCamera.hpp"

#define NOM_FENETRE "CamCaptor"

using namespace std;


int main()
{

    //UTCamera *camAv = new UTCamera(CAMERA_AVA);
    UTCamera *camAv = new UTCamera(CAMERA_ARR);

    camAv->calibrage();




    return 0;
}
