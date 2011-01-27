/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    UTCoupe 2010-2011
    All rights reserved.

*   Gére la communication avec l'intelligence
*****************************************************
 **/

#include "cam_comManager.hpp"

int waitOrder()
{
    int c;
    cin >> c;
    fflush(stdin);
    return c;
}

void send(regionLister* rl)
{
    int compteur=0;
    region* rg;

    rl->iniPtr();
    while(rl->getEle()!=NULL){
        rg = rl->getEle();
        compteur++;
        // ----------
        cout << rg->Type << " " << (int)rg->Xprime << " " << (int)rg->Yprime << ",";
        // ----------
        rl->ptrSuiv();
    }
    if(compteur<1){
        cout << "r" << ",";
    }
}
