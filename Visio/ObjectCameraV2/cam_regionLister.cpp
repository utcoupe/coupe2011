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
#include "cam_regionLister.hpp"

// **************************************************
// ***************** CONSTRUCTEUR *******************
regionLister::regionLister()
{
    deb = NULL;
    fin = NULL;
}

// **************************************************
// ** Methode d'ajout d'une region à la liste      **
void regionLister::add(region* r)
{
    r->suiv = NULL;
    if(deb==NULL) // Si la liste est vide
	{
		deb 	  = r  ;
		fin 	  = deb;
	}else{        // Sinon
		fin->suiv = r;
		fin 	  = fin->suiv;
	}
}

// **************************************************
// ** Methode de destruction de la liste           **
void regionLister::destroy()
{
    region *tmp,*t;
    tmp = deb;
    while(tmp!=NULL){
        t=tmp->suiv;
        free(tmp);
        tmp=t;
    }
    deb = NULL;
    fin = NULL;
}

// **************************************************
// ** Methode qui permet de naviguer dans la liste **
// ** Utilisez initPtr pour revenir au début       **
void regionLister::ptrSuiv()
{
    ptr = ptr->suiv;
}

// **************************************************
// ******************* DESTRUCTEUR ******************
regionLister::~regionLister()
{
    destroy();
}
