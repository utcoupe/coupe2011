#ifndef colorConvertion_H
#define colorConvertion_H
/**
 * \file colorConvertion.hpp
 * \brief Algorithmes de convertion de couleur
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

    /*** EXTERN GLOBAL ***/

extern IplImage*       imgBas;
extern IplImage*       imgBin;
extern IplImage*       imgHsv;
extern IplImage*       imgBin2;

extern int             _H[NB_COLOR];
extern int             _Htol[NB_COLOR];
extern int             _S[NB_COLOR];
extern int             _Stol[NB_COLOR];

extern pthread_t       th[NB_THREAD];

    /*** STRUCTURE ***/

typedef struct ThreadData{

    int firstRow;
    int nbRowTotrait;

}threaddata;

    /*** PROTOTYPE ***/

void binarisation();
void binarisation2();
void* binThread(void* pData);
void* binThread2(void* pData);

#endif
