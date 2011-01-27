#include <iostream>

#include "cv.h"
#include "highgui.h"

#include "UTCamera.hpp"

#define NOM_FENETRE "CamCaptor"


using namespace std;


#include "cam_regionAnalyser.hpp"

void* activCamera(void* pData)
{
    int id = (int)pData;
    UTCamera *cam = new UTCamera(id, 1280, 800);
    cam->generalRun();
    //cam->calibrage();
}

int main()
{

    // ***************
    /*
    pthread_t cameraAvant;
    pthread_t cameraArriere;

    pthrais bon c'est pas grave on fera la vidéo demain au réveil (13-14h) à ce qu'il parrait les vidéos sont plus réussi à la lumiére du jour... ;)

Bonne fin de nuit les petits...ead_create(&cameraAvant  , NULL, activCamera, (void *)CAMERA_AVA);
    pthread_create(&cameraArriere, NULL, activCamera, (void *)CAMERA_ARR);

    pthread_join (cameraAvant, NULL);
    pthread_join (cameraArriere, NULL);
    */
    // ***************


    //fenetreDeParametrageDesCourbes(CAMERA_AVA);

   // parametrage(CAMERA_ARR);

    //double a, b, c;
    /*computeCoef(&a, &b, &c,
                 230, 480, 490, 240, 1640, 0);

*/
    //UTCamera *ctour à la ligneamAv = new UTCamera(CAMERA_TEST, 1280, 800);

    UTCamera *camAv = new UTCamera(CAMERA_AVA, 1280, 800);

    //camAv->calibrage();
    camAv->generalRun();



    //camAv->record("vtest.avi",VISION_NORMAL);

    return 0;
}



