/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    cam_utility.cpp
    UTCoupe 2010-2011
    All rights reserved.

*   Ensemble de fonctions simples mais pratiques
*****************************************************
 **/
#include "cam_utility.hpp"
/****************************************************
*****              FONCTIONS                    *****
****************************************************/
string intToString(int entier)
{
    ostringstream out;
    out << entier;
    return out.str();
}

int chareToInt(char* car)
{
    return atoi(car);
}



