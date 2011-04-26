/**
 * \file UTCamera.cpp
 * \brief fichier principale du projet vision
 * \version 1
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
#include "UTCamera.hpp"

/**
 * \fn int main(int argc, char* argv[])
 * \brief point d'entrée du programme
*/
int main(int argc, char* argv[])
{
    /*
    */
    identificationCamera();
    /*
    */
    initGlobalVariable();
    /*
    */
    //calibrationManuel();
    modeStationnaire();

    return 0;
}



/**
 * \fn void initGlobalVariable()
 * \brief initialisation des variables globales du programmes
 */
void initGlobalVariable()
{
    imgBin  = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_8U, 1);
    imgBin2 = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_8U, 1);
    imgHsv  = cvCreateImage(cvSize(IMG_WIDTH, IMG_HEIGHT), IPL_DEPTH_8U, 3);

    listBeforeTrait = new regionLister();
    listFinal       = new regionLister();

    loadConfig();
}
