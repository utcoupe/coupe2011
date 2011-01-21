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
#include <iostream>
using namespace std;
#include <stdlib.h>
#include <stdio.h>

// OpenCV
#include <cv.h>
using namespace cv;
#include <highgui.h>

// Librairie Interne
#include "ListeRegion.hpp"

/****************************************************
*****           MACRO-DEFINITION                *****
****************************************************/
// Choix de la caméra
#define CAMERA_AVA 0
#define CAMERA_ARR 1

//
#define PIONT
#define FIGUR

// Valeurs identifiants les couleurs
#define NB_COLOR            7

#define UNDEFINE 			0
#define	NOIR				1
#define ROUGE 				2
#define VERT				3
#define	BLEU				4
#define	JAUNE				5
#define	BLANC				6

#define TEXTE               "num : HSV\n1) NOIR\n2) ROUGE\n3) VERT\n4) BLEU\n5) JAUNE\n6) BLANC"

// Valeurs identifiants les positions des maximum des regions
#define HAUT 	            0
#define BAS 	            1
#define DROITE 	            2
#define GAUCHE 	            3

// Mode d'enregistrement
#define VISION_NORMAL       0
#define VISION_MASK         1
#define VISION_ZONE         2

// Limte de recursivité
#define STOP_REC            100000

// Calcul des seuils avec la tolerance
#define SEUIL(nb, cherche, tol) (nb > (cherche-tol) && nb < (cherche+tol))
// Renvoie true si la couleur en hsv est dans les seuils de tolerance de color.
#define IS_COLOR(h,s,v,color)   (SEUIL( h, Htab[color], Htol[color]) && SEUIL( s, Stab[color], Stol[color])   && SEUIL( v, Vtab[color], Vtol[color]))
// Division par 2 avec prise en compte du reste
#define DIV2SUP(x) 		( (x%2)==0 ?   ((x)/2) : (((x)+1)/2) )
#define DIV2INF(x) 		( (x%2)==0 ?   ((x)/2) : (((x)-1)/2) )

/****************************************************
*****           PROTOTYPES                      *****
****************************************************/


/****************************************************
*****           UTCamera                        *****
****************************************************/

class UTCamera
{

    public:

        UTCamera(int camera, int wScreen, int hScreen);          // Utiliser les macros pour choisir la caméra

        //--- Methodes pour l'utilisateur ---
        void calibrage();

        int* analyseZoneVerte();                    // Retourne un tableau de 5 entiers indiquant l'ordre des pionts
                                                    // et des figures sur la zone verte.
                                                    // 0 -> plus proche  4 -> plus loin


        void record(int mode);                      // Pour enregistrer la vidéo produite

        //--- Methodes Avancées ---
        void maskApplication();
        void RegGApplication();

        void afficherMask();
        void afficherMask(char* WindowMaskName);
        void afficherRegi();

        void saveConfig();
        void loadConfig();

        string      getName()                   {return nameCam;    };
        CvCapture*  getCapture()                {return ptrCamera;  };
        IplImage*   getFrame()                  {return frame;      };

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

        string          nameCam;
        int             width;
        int             height;

        int             wScreen;
        int             hScreen;

        CvCapture*      ptrCamera;
        IplImage*       frame;
        unsigned char** imgTab;
        ListeRegion    listeR;

        int ind;

        int Htab[7];
        int Htol[7];
        int Stab[7];
        int Stol[7];
        int Vtab[7];
        int Vtol[7];

};

#endif
