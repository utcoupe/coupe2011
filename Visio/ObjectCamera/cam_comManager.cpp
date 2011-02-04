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

char waitOrder()
{
    char c;
    cin >> c;
    fflush(stdin);
    return c;
}

void send(regionLister* rl)
{
    int compteur=0;
    region* rg;

    cout << "1 ," ;

    rl->iniPtr();
    while(rl->getEle()!=NULL){
        rg = rl->getEle();
        compteur++;
        // ----------
        if(compteur>1){cout << ";";}
        cout << rg->Type << " " << (int)rg->Yprime << " " << (int)rg->Xprime;
        // ----------
        rl->ptrSuiv();
    }
    if(compteur<1){
        cout << "r" ;
    }
    cout << endl;
}
