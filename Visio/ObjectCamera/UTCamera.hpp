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

// Valeurs identifiants les couleurs
#define NB_COLOR            7

#define UNDEFINE 			0
#define	NOIR				1
#define ROUGE 				2
#define VERT				3
#define	BLEU				4
#define	JAUNE				5
#define	BLANC				6

// Valeurs identifiants les positions des maximum des regions
#define HAUT 	            0
#define BAS 	            1
#define DROITE 	            2
#define GAUCHE 	            3

// Calcul des seuils avec la tolerance
#define seuil(nb, cherche, tol) (nb > (cherche-tol) && nb < (cherche+tol))

/****************************************************
*****           PROTOTYPES                      *****
****************************************************/
void checkCouleur(int);



class UTCamera
{

    public:

        UTCamera(int camera);          // Utiliser les macros pour choisir la caméra

        //--- Methodes pour l'utilisateur ---
        void calibrage();



        //--- Methodes Avancées ---
        void maskApplication();
        void RegGApplication();

        void afficherMask();
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
