/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr
    FRICOT      Pierre-Henri

    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet de gérer les listes chainées de
*   régions
*****************************************************
 **/
#ifndef Liste_Region
#define Liste_Region

/****************************************************
*****           INCLUDES                        *****
****************************************************/
#include <iostream>
#include <cv.h>

/****************************************************
*****           STRUCTURE                       *****
****************************************************/
typedef struct Region{
	CvPoint		 	max[4];		// Coins de la zone
								// | 0 | 1 |
								// | 2 | 3 |
	int 			nbPixel;
	unsigned char 	color;		// Couleur des pixels de la zone
	// Si la région contient du jaune
	unsigned char	type;

	// -----
    struct Region *suiv;

}region;


/****************************************************
*****           ListeRegion                     *****
****************************************************/

class ListeRegion
{
    public:

        ListeRegion();

        void iniPtr()       {ptr=deb;};
        void ptrSuiv();
        region* getEle()    {return ptr;};
        void add(region* r);
        void destroy();

        ~ListeRegion();

    private:

        region* deb;
        region* fin;
        region* ptr;

};



#endif
