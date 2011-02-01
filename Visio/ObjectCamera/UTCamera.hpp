/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr
    FRICOT      Pierre-Henri

    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet de gérer les caméras du robot.
*   Permet de récupérer les informations importantes
*   de l'environnement grâce à du traitement d'image.

*   Distance en mm
*****************************************************
 **/

#ifndef UTC_amera
#define UTC_amera
/****************************************************
*****           INCLUDES                        *****
****************************************************/
// System
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

// OpenCV
#include <cv.h>
using namespace cv;
#include <highgui.h>

// Extern
#include "cam_regionLister.hpp"
#include "cam_regionAnalyser.hpp"
#include "cam_comManager.hpp"
/****************************************************
*****           MACRO-DEFINITION                *****
****************************************************/
// Choix de la caméra
#define CAMERA_TEST                 0
#define CAMERA_AVA                  1
#define CAMERA_ARR                  2

// Valeurs identifiants les couleurs
#define NB_COLOR                    7

#define UNDEFINE 			        0
#define	NOIR				        1
#define ROUGE 				        2
#define VERT				        3
#define	BLEU				        4
#define	JAUNE				        5
#define	BLANC				        6

#define TEXTE                       "num : HSV\n1) NOIR\n2) ROUGE\n3) VERT\n4) BLEU\n5) JAUNE\n6) BLANC"



// Mode d'application du masque
#define MASK_BINARISATION           0
#define MASK_COLOR                  1

// Mode d'enregistrement
#define VISION_NORMAL               0
#define VISION_BINAR                1
#define VISION_COLOR                2
#define VISION_ZONE                 3

// Limte de recursivité
#define STOP_REC                    100000
// Nombre de thread maximum
#define NB_THREAD                   4

// Vérifie si la couleur rgb du pixel est bien la même que les paramétres
#define CHECK_COLOR(f,x,y,r,g,b)   (CV_IMAGE_ELEM( f, uchar, y, ( x * 3 )   ) == b && \
                                    CV_IMAGE_ELEM( f, uchar, y, ( x * 3 ) +1) == g && \
                                    CV_IMAGE_ELEM( f, uchar, y, ( x * 3 ) +2) == r    )

// Division par 2 avec prise en compte du reste
#define DIV2SUP(x) 		           ( (x%2)==0 ?   ((x)/2) : (((x)+1)/2) )
#define DIV2INF(x) 		           ( (x%2)==0 ?   ((x)/2) : (((x)-1)/2) )

/****************************************************
*****           PROTOTYPES                      *****
****************************************************/

// **************************************************
// *         Structure utilisée par les threads     *
typedef struct ThreadData{
    int firstRow;   // Premiére ligne à traiter par le thread
    int nbRow;      // Nombre de ligne à traiter

    int mode;

    int width,height;

    int *Htab,*Htol,*Stab;
    int *Stol,*Vtab,*Vtol;

    IplImage* src;
    IplImage* dst;
}threaddata;

/****************************************************
*****           UTCamera                        *****
****************************************************/

class UTCamera
{

    public:

        UTCamera(int camera, int orentation, int wScreen, int hScreen);          // Utiliser les macros pour choisir la caméra

        //--- Methodes pour l'utilisateur ---
        void calibrage();
        void autoCalibrage();

        int* analyseZoneVerte();                    // Retourne un tableau de 5 entiers indiquant l'ordre des pionts
                                                    // et des figures sur la zone verte.
                                                    // 0 -> plus proche  4 -> plus loin


        void generalRun();

        void record(char* nom, int mode);           // Pour enregistrer la vidéo produite

        //--- Methodes Avancées ---
        void        maskApplication(int mode);
        void        reggApplication();
        void        analyseRegion();

        void        afficherMask();
        void        afficherRegi();

        void        saveConfig();
        void        loadConfig();

        string      getName()                   {return nameCam;   };
        CvCapture*  getCapture()                {return ptrCamera; };
        IplImage*   getFrame()                  {return basFrame;  };

        void        setHtab(int ind, int val)   {Htab[ind]=val;};
        void        setHtol(int ind, int val)   {Htol[ind]=val;};
        void        setStab(int ind, int val)   {Stab[ind]=val;};
        void        setStol(int ind, int val)   {Stol[ind]=val;};
        void        setVtab(int ind, int val)   {Vtab[ind]=val;};
        void        setVtol(int ind, int val)   {Vtol[ind]=val;};

        int         getHtab(int ind)            {return Htab[ind];};
        int         getHtol(int ind)            {return Htol[ind];};
        int         getStab(int ind)            {return Stab[ind];};
        int         getStol(int ind)            {return Stol[ind];};
        int         getVtab(int ind)            {return Vtab[ind];};
        int         getVtol(int ind)            {return Vtol[ind];};

        int         getInd()                    {return ind;};

        ~UTCamera();

    private:

        int             idCamera;
        string          nameCam;    // Nom des fenetres de l'objet
        CvCapture*      ptrCamera;

        int             width;      // Résolution de l'image venant de la caméra
        int             height;
        int             wScreen;    // Résolution de l'écran
        int             hScreen;

        IplImage*       basFrame;   // Image de base
        IplImage*       hsvFrame;   // Image converti en hsv
        IplImage*       colFrame;   // Image colorisée
        IplImage*       binFrame;   // Image binarisée

        regionLister*     listeR;   // Liste des Régions primaires
        regionLister*     listeP;   // Liste des pionts

        threaddata**    data;       // Structure pour le multithread
        pthread_t       th[NB_THREAD];

        CvFont font;

        int ind;

        int Htab[7];
        int Htol[7];
        int Stab[7];
        int Stol[7];
        int Vtab[7];
        int Vtol[7];

        int nbPiont;

};

#endif
