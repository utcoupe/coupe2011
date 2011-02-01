/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    cam_regionAnalyser.cpp
    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet d'analyser une liste de region:
*       - Détermine si la zone se compose de n sous
*         zones
*       - Détermine le type de la zone (piont...)
*       - Détermine les distances en mm qui séparent
*         le robot de l'objectif en x et y relatif.
*****************************************************
 **/
/**       NORMES DANS CE FICHIER                   **
*****************************************************
Distance en mm
x' et y' coordonnées relatives aux robots
                  x'
              |---------O obejctif
           y' |
              |
            ----
            |  | robot
            ----
*****************************************************
**/
#include "cam_regionAnalyser.hpp"

/****************************************************
*****              FONCTIONS                    *****
****************************************************/
/* Fonction qui permet d'analyser une liste de
*  regions et de déterminer pour chacune d'elle le
*  type, la distance par rapport au robot et le décalage
*
*  @param une liste à analyser
*  @return la liste des pionts et tours
*/
regionLister* analyseListeRegion(regionLister* listeAanalyser, int width, int height)
{
    // Déclaration
    region* reg;
    regionLister* liste = new regionLister();

    listeAanalyser->iniPtr();
    // Boucle de traitement
    while(listeAanalyser->getEle()!=NULL){
        reg = (region*)malloc(sizeof(region));
        reg->nbPixel = listeAanalyser->getEle()->nbPixel;
        for(int i=0 ; i<4 ; i++){
            reg->max[i] = listeAanalyser->getEle()->max[i];
        }
        // ------------
        reg->Type = TYPE_PIONT;
        reg->Yprime = getLengthRobotToRegion(reg->max[BAS].y);

        double largeurTheorique = getLargeurTheoriqueDuPiont(reg->max[BAS].y + ((2/3)*(reg->max[BAS].y-reg->max[HAUT].y)) );
        double taillePix = 200.0f / largeurTheorique;

        reg->Xprime = (width/2) - (reg->max[GAUCHE].x + (largeurTheorique/2));
        reg->Xprime = reg->Xprime * taillePix;
        reg->Xprime = (-1)*reg->Xprime;

        liste->add(reg);
        // ------------
        listeAanalyser->ptrSuiv();
    }
    return liste;
}


/* Fonction qui permet de retrouver grâce au pixel bas
*  d'une region sa largeur théorique en Pixel
*
*  @param  ordonnée basse de la zone en pixel
*  @return largeurThéorique          en pixel
*/
#define COEF_A1 0.6273
#define COEF_B1 90.36
double getLargeurTheoriqueDuPiont(int yDuBasDuPiont)
{
    return (COEF_A1*yDuBasDuPiont) + COEF_B1;
}

/* Fonction qui permet de retrouver séparant
*  le robot de la zone grâce au point bas de
*  la region.
*
*  @param  ordonnée basse de la zone en pixel
*  @return distance robot region     en mm
*/
#define COEF_A2 -406
#define COEF_B2 2709
double getLengthRobotToRegion(int yDuBasDuPiont)
{
    return (COEF_A2*log(yDuBasDuPiont)) + COEF_B2 + RAYON_PIONT;
}


// **************************************************
// ** Fonction d'aide au paramétrage du code       **
void fenetreDeParametrageDesCourbes(int camera)
{
    // Déclaration et initialisation
    STD_POLICE

    IplImage*  frame;
    CvCapture* paraCap = cvCaptureFromCAM(camera);

    string text1,text2;
    int ligne    = 240;
    int largeur1 = 100;
    int largeur2 = 300;
    double distance, diffLargeur;

    // Configuration de la fenetre
    cvNamedWindow( "Parametrage" , CV_WINDOW_AUTOSIZE );

    cout << "Appuyer sur 'q' pour quitter" << endl;
    while(1){
        frame = cvQueryFrame(paraCap);
        if(frame==NULL){continue;}
        // -----

        // Création des trackbar
        cvCreateTrackbar( "Ligne d'horizon", "Parametrage", &ligne,    frame->height, NULL);
        cvCreateTrackbar( "Largeur1 piont",  "Parametrage", &largeur1, frame->width,  NULL);
        cvCreateTrackbar( "Largeur2 piont",  "Parametrage", &largeur2, frame->width,  NULL);

        // Calculs
        distance = getLengthRobotToRegion(ligne);
        diffLargeur = ABS(largeur2 - largeur1);

        // Affichage
        text1 = "Distance = " + intToString(distance) + " mm";
        if(ligne>30){
            cvPutText(frame,text1.c_str(),cvPoint( 0,ligne-10 ), &font, cvScalar(0,255,255));
        }else{
            cvPutText(frame,text1.c_str(),cvPoint( 0,ligne+15 ), &font, cvScalar(0,255,255));
        }

        text2 = "Difference = " + intToString(diffLargeur) + " pixel";
        cvPutText(frame,text2.c_str(),cvPoint(frame->width-200, 20), &font, cvScalar(255,255,0));

        cvLine(frame, cvPoint(0,ligne), cvPoint(frame->width,ligne), cvScalar(0,255,255), 1);
        cvLine(frame, cvPoint(largeur1,0), cvPoint(largeur1,frame->height), cvScalar(255,255,0), 1);
        cvLine(frame, cvPoint(largeur2,0), cvPoint(largeur2,frame->height), cvScalar(255,255,0), 1);

        cvShowImage( "Parametrage", frame );
        // -----
        char c = cvWaitKey(10);
        if( c == 'q' ) break;
    }

}


// **************************************************
// ** Fonction qui permet d'afficher une liste de  **
// ** region sur une image, avec leurs caractéristiques
void afficheListeRegion(regionLister* rl, IplImage* imgPtr)
{
    string text1;
    region* rg;
    STD_POLICE

    rl->iniPtr();
    while(rl->getEle()!=NULL){
        rg = rl->getEle();
        // ----------

        text1 = rg->Type;
        text1 = "Type : "+ text1  +" Y' = " + intToString(rg->Yprime) + " mm"+" X' = " + intToString(rg->Xprime);
        cvPutText (imgPtr,text1.c_str(),cvPoint( (rg->max[DROITE].x+rg->max[GAUCHE].x)/2,(rg->max[HAUT].y+rg->max[BAS].y)/2 ), &font, cvScalar(255,0,255));

        cvLine(imgPtr, rg->max[HAUT], rg->max[DROITE], cvScalar(255,0,255), 3);
        cvLine(imgPtr, rg->max[HAUT], rg->max[GAUCHE], cvScalar(255,0,255), 3);
        cvLine(imgPtr, rg->max[BAS ], rg->max[DROITE], cvScalar(255,0,255), 3);
        cvLine(imgPtr, rg->max[BAS ], rg->max[GAUCHE], cvScalar(255,0,255), 3);

        // ----------
        rl->ptrSuiv();
    }
}




