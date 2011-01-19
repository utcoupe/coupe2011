#include "ListeRegion.hpp"

ListeRegion::ListeRegion()
{
    deb = NULL;
    fin = NULL;
}

void ListeRegion::add(region* r)
{
    r->suiv = NULL;
    if(deb==NULL)
	{
		deb 	  = r  ;
		fin 	  = deb;
	}else{
		fin->suiv = r;
		fin 	  = fin->suiv;
	}
}

void ListeRegion::destroy()
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

void ListeRegion::ptrSuiv()
{
    ptr = ptr->suiv;
}

ListeRegion::~ListeRegion()
{
    destroy();
}
