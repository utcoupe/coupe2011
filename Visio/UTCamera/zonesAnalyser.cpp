    /*** INCLUDE ***/
#include "zonesAnalyser.hpp"
#include "cam_utility.hpp"
#include "regionLister.hpp"
#include <math.h>

/**
 * \fn regionLister* analyseListeRegion(regionLister* listeAanalyser, int width, int height)
 * \brief   Fonction qui permet d'analyser une liste de
 *          regions et de déterminer pour chacune d'elle le
 *          type, la distance par rapport au robot et le décalage
 *
 * \param une liste à analyser
 * \return la liste des pionts et tours
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

       // reg->Xprime = (width/2) - (reg->max[GAUCHE].x + (largeurTheorique/2));
        reg->Xprime = (width/2) - (reg->max[GAUCHE].x + reg->max[DROITE].x)/2;
        reg->Xprime = reg->Xprime * taillePix ;
       // reg->Xprime = (-1)*reg->Xprime;

        liste->add(reg);
        // ------------
        listeAanalyser->ptrSuiv();
    }

    return liste;
}


/**
 * \fn void afficheListeRegion(regionLister* rl, IplImage* imgPtr)
 * \brief affiche la liste des regions sur l'image
 */
void afficheListeRegion(regionLister* rl, IplImage* imgPtr)
{
    string text1;
    region* rg;
    STD_POLICE

    rl->iniPtr();
    while(rl->getEle()!=NULL){
        rg = rl->getEle();
        // ----------
        if(rg->Type == TYPE_TOUR){
            text1 = rg->Type;
            text1 = "Type : "+ text1  +" Y' = " + intToString(rg->Yprime) + " mm"+" X' = " + intToString(rg->Xprime);
        }
        if(rg->Type == TYPE_PIONT){
            text1 = rg->Type;
            text1 = "Type : "+ text1  +" Y' = " + intToString(rg->Yprime) + " mm"+" X' = " + intToString(rg->Xprime);
        }
        cvPutText (imgPtr,text1.c_str(),cvPoint( (rg->max[DROITE].x+rg->max[GAUCHE].x)/2,(rg->max[HAUT].y+rg->max[BAS].y)/2 ), &font, cvScalar(0,0,0));
        cvLine(imgPtr, rg->max[HAUT], rg->max[DROITE], cvScalar(0,0,0), 3);
        cvLine(imgPtr, rg->max[HAUT], rg->max[GAUCHE], cvScalar(0,0,0), 3);
        cvLine(imgPtr, rg->max[BAS ], rg->max[DROITE], cvScalar(0,0,0), 3);
        cvLine(imgPtr, rg->max[BAS ], rg->max[GAUCHE], cvScalar(0,0,0), 3);
        // ----------
        rl->ptrSuiv();
    }
}



/**
 * \fn void extendRegion(region* z, int width, int height, IplImage* binFrame, int x, int y, int stop)
 * \brief Fonction pour étendre une zone
 */
void extendRegion(region* z, int width, int height, IplImage* binFrame, int x, int y, int stop)
{

    if(stop<STOP_REC){
		(z->nbPixel)++;
        CV_IMAGE_ELEM( binFrame, uchar, y, x ) = UNDEFINE_;

		if( y < z->max[HAUT     ].y ) { z->max[HAUT     ] = cvPoint(x,y); }
		if( y > z->max[BAS      ].y ) { z->max[BAS      ] = cvPoint(x,y); }
		if( x > z->max[DROITE   ].x ) { z->max[DROITE   ] = cvPoint(x,y); }
		if( x < z->max[GAUCHE   ].x ) { z->max[GAUCHE   ] = cvPoint(x,y); }

		if( (x+1<width) ) {
            if( CV_IMAGE_ELEM( binFrame, uchar, y, (x+1) ) == JAUNE_ ){
                extendRegion(z, width, height, binFrame, x+1, y, stop+1);
			}
		}
		if( (x-1>=0) ) {
		    if( CV_IMAGE_ELEM( binFrame, uchar, y, (x-1) ) == JAUNE_ ){
                extendRegion(z, width, height, binFrame, x-1, y, stop+1);
            }
		}
		if( (y+1<height) ) {
		    if( CV_IMAGE_ELEM( binFrame, uchar, (y+1), x ) == JAUNE_ ){
                extendRegion(z, width, height, binFrame, x, y+1, stop+1);
            }
            //*
            else if(y+4<height && CV_IMAGE_ELEM( binFrame, uchar, (y+4), x ) == JAUNE_){
                extendRegion(z, width, height, binFrame, x, y+4, stop+1);
            }
            //*/
		}
		if( (y-1>=0) ) {
            if( CV_IMAGE_ELEM( binFrame, uchar, (y-1), x ) == JAUNE_ ){
                extendRegion(z, width, height, binFrame, x, y-1, stop+1);
            }
            //*
            else if(y-4>=0 && CV_IMAGE_ELEM( binFrame, uchar, (y-4), x ) == JAUNE_){
                extendRegion(z, width, height, binFrame, x, y-4, stop+1);
            }
            //*/
		}

    }

}



/**
 * \fn void regionGrowing()
 * \brief Methode pour appliquer le region growing et remplir la liste des regions de l'objet
 */
void regionGrowing()
{
    listBeforeTrait->destroy();
    region *r; int c;

	for( int x=0 ; x<imgBin->width ; x++ ){
		for( int y=0 ; y<imgBin->height ; y++ ){
                c = CV_IMAGE_ELEM( imgBin, uchar, y, x );

			if(c==JAUNE_)
			{
			    r = (region*)malloc(sizeof(region));

				r->max[HAUT  ] = cvPoint(x,y);
				r->max[BAS   ] = cvPoint(x,y);
				r->max[DROITE] = cvPoint(x,y);
				r->max[GAUCHE] = cvPoint(x,y);
				r->color       = c;
				r->nbPixel     = 0;

				extendRegion(r, imgBin->width, imgBin->height, imgBin, x, y, 0);

				if( (r->nbPixel > 1000) )
				{
				    listBeforeTrait->add(r);
				}
				else
				{
                     free(r);
				}
			}
		}
	}
}



/**
 * \fn double getLargeurTheoriqueDuPiont(int yDuBasDuPiont)
 * \brief   Fonction qui permet de retrouver grâce au pixel bas
 *          d'une region sa largeur théorique en Pixel
 *
 * \param   ordonnée basse de la zone en pixel
 * \return  largeurThéorique          en pixel
 */
#define COEF_A1 0.6273
#define COEF_B1 90.36
double getLargeurTheoriqueDuPiont(int yDuBasDuPiont)
{
    return (COEF_A1*yDuBasDuPiont) + COEF_B1;
}

/**
 * \fn double getLengthRobotToRegion(int yDuBasDuPiont)
 * \brief   Fonction qui permet de retrouver séparant
 *          le robot de la zone grâce au point bas de
 *          la region.
 *
 * \param  ordonnée basse de la zone en pixel
 * \return distance robot region     en mm
 */
#define COEF_A2 -406
#define COEF_B2 2709
double getLengthRobotToRegion(int yDuBasDuPiont)
{
    return (COEF_A2*log(yDuBasDuPiont)) + COEF_B2 + RAYON_PIONT;
}



