#ifndef UTCamera
#define UTCamera

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <cv.h>
#include <highgui.h>

/******************************************
************ MACRO DEFINITION *************
*******************************************/
// Choix de la caméra
#define CAMERA_AVA 0
#define CAMERA_ARR 1

using namespace std;

// Numéro identifiant la couleur
#define UNDEFINE 			0
#define	NOIR				1
#define ROUGE 				2
#define VERT				3
#define	BLEU				4
#define	JAUNE				5
#define	BLANC				6

// Macro Fonctions
#define seuil(nb, cherche, tol) (nb > (cherche-tol) && nb < (cherche+tol))

void checkCouleur(int);

class UTCam
{

    public:

        UTCam(int camera);

        void calibrage();
        void maskApplication();

        void afficherMask();

        string      getName()   {return nameCam;    };
        CvCapture*  getCapture(){return ptrCamera;  };
        IplImage*   getFrame()  {return frame;      };


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

        ~UTCam();

    private:

        string          nameCam;
        int             width;
        int             height;

        CvCapture*      ptrCamera;
        IplImage*       frame;
        unsigned char** imgTab;

        int Htab[7];
        int Htol[7];
        int Stab[7];
        int Stol[7];
        int Vtab[7];
        int Vtol[7];

        int ind;


};

#endif
