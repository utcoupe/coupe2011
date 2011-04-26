/**
 * \file colorConvertion.cpp
 * \brief Algorithmes de convertion de couleur
 * \version 1
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
#include "colorConvertion.hpp"

/**
 * \fn binarisation
 * \brief Fonction pour binariser l'image de base
*/
void binarisation()
{
    // Image temporaire convertie en hsv
    if(imgBas != NULL){
    cvCvtColor(imgBas, imgHsv, CV_BGR2HSV);}

    // Lancement des threads
    threaddata**    data;
    data = (threaddata**)malloc(sizeof(threaddata*)*NB_THREAD);
    for(int i=0 ; i<NB_THREAD ; i++){
        data[i] = (threaddata*)malloc(sizeof(threaddata));
    }

    for(int i=0 ; i<NB_THREAD ; i++){
        data[i]->firstRow = i*(IMG_HEIGHT/NB_THREAD);
        data[i]->nbRowTotrait = (IMG_HEIGHT/NB_THREAD);

        pthread_create((th+i), NULL, binThread, (void*)*(data+i));
    }

    // Récupération et synchronisation
    for(int i=0 ; i<NB_THREAD ; i++){
        pthread_join (th[i], NULL);
    }
}


void* binThread(void* pData)
{
    // Récupération des données
    int h,s,v,ymax,val;
    threaddata* data = (threaddata*)pData;
    ymax = data->firstRow + data->nbRowTotrait;

    // Traitement
    for(int x=0 ; x<IMG_WIDTH ; x=x+1){
        for(int y=data->firstRow ; y<ymax ; y=y+1){

            // Valeur du pixel
            h = CV_IMAGE_ELEM( imgHsv, uchar, y, ( x * 3 )   );
            s = CV_IMAGE_ELEM( imgHsv, uchar, y, ( x * 3 ) +1);

            if(IS_COLOR(h,s,JAUNE))
                {val=JAUNE_;}
            else
                {val=UNDEFINE_;}

            SET1_PIXEL(imgBin,x,y,val);
        }
    }

    return pData;
}



/**
    Fonction pour binariser l'image de base // pour le debug et la calibration
*/
void binarisation2()
{
    // Image tempocvWaitKey(0);raire convertie en hsv
    cvCvtColor(imgBas, imgHsv, CV_BGR2HSV);

    // Lancement des threads
    threaddata**    data;
    data = (threaddata**)malloc(sizeof(threaddata*)*NB_THREAD);
    for(int i=0 ; i<NB_THREAD ; i++){
        data[i] = (threaddata*)malloc(sizeof(threaddata));
    }

    for(int i=0 ; i<NB_THREAD ; i++){
        data[i]->firstRow = i*(IMG_HEIGHT/NB_THREAD);
        data[i]->nbRowTotrait = (IMG_HEIGHT/NB_THREAD);

        pthread_create((th+i), NULL, binThread2, (void*)*(data+i));
    }

    // Récupération et synchronisation
    for(int i=0 ; i<NB_THREAD ; i++){
        pthread_join (th[i], NULL);
    }

}


void* binThread2(void* pData)
{
    // Récupération des données
    int h,s,v,ymax,val;
    threaddata* data = (threaddata*)pData;
    ymax = data->firstRow + data->nbRowTotrait;

    // Traitement
    for(int x=0 ; x<IMG_WIDTH ; x=x+1){
        for(int y=data->firstRow ; y<ymax ; y=y+1){

            // Valeur du pixel
            h = CV_IMAGE_ELEM( imgHsv, uchar, y, ( x * 3 )   );
            s = CV_IMAGE_ELEM( imgHsv, uchar, y, ( x * 3 ) +1);

            if(IS_COLOR(h,s,JAUNE))
                {val=JAUNE_;}
            else
                {val=UNDEFINE_;}

            SET1_PIXEL(imgBin2,x,y,val);
        }
    }
    return pData;
}


