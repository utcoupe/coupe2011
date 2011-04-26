/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    cam_regionLister.hpp
    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet de gérer les listes chainées de
*   régions
*****************************************************
 **/
#ifndef _regionLister
#define _regionLister
/****************************************************
*****           INCLUDES                        *****
****************************************************/
#include <iostream>
#include <cv.h>

#include "constantes.hpp"

/****************************************************
*****           STRUCTURE                       *****
****************************************************/
typedef struct Region{
	CvPoint		 	max[4];		// Coins de la zone
								//           | HAUT |
								//  | GAUCHE |      | DROITE |
								//           | BAS  |
	int 			nbPixel;
	int            	color;		// Couleur des pixels de la zone
    // -----
    // Décalage pour atteindre la zone
    double          Xprime;     // Décalage en mm a droite (si >0) ou à gauche si (si <0), sens horaire
    double          Yprime;     // Décalage vers l'avant en mm
    char            Type;
	// -----
    struct Region   *suiv;
}region;

/****************************************************
*****           regionLister                    *****
****************************************************/
class regionLister
{
    public:

        regionLister    ();

        void    iniPtr  ()        {ptr=deb;};
        void    ptrSuiv ();
        region* getEle  ()        {return ptr;};
        void    add(region* r);
        void    destroy ();

        region* copie(region* r);

        ~regionLister();

    private:

        region* deb;
        region* fin;
        region* ptr;
};

#endif
