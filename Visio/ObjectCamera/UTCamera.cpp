/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr
    FRICOT      Pierre-Henri

    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet de gérer les caméras du robot.
*   Permet de récupérer les informations importantes
*   de l'environnement grâce à du traitement d'image.

*   Distance en mm
*****************************************************
 **/
#include "UTCamera.hpp"


/**       NORMES DANS CE FICHIER                   **
*****************************************************
Coordonnées dans l'image.
        0      x ou i = width
      0 -------------------------
        |                       |
        |                       |
    y   |                       |
    ou  |                       |
    j   |                       |
=height |                       |
        -------------------------

Pour l'accés dans une IplImage :
CV_IMAGE_ELEM( hsv, uchar, y, ( x * 3 ) + n );

*****************************************************
**/

/****************************************************
*****       FONCTIONS DE LA TRACKBAR            *****
****************************************************/

// Fonction qui affiche sur la console la couleur à calibrer
void checkCouleur(int valeur) {
    switch(valeur) {
    case UNDEFINE:  cout<<"UNDEFINED"<<endl;break;
    case NOIR:      cout<<"NOIR"<<endl;     break;
    case ROUGE:     cout<<"ROUGE"<<endl;    break;
    case VERT:      cout<<"VERT"<<endl;     break;
    case BLEU:      cout<<"BLEU"<<endl;     break;
    case JAUNE:     cout<<"JAUNE"<<endl;    break;
    case BLANC:     cout<<"BLANC"<<endl;    break;
    }
}

// Fonctions qui récupére la position de la souris et calibre la couleur séléctionnée
void souris(int event, int x, int y, int flags, void *param) {

    if(event == CV_EVENT_LBUTTONUP) {
        IplImage* f;
        UTCamera* cam = (UTCamera*)param;

        f=cvRetrieveFrame(cam->getCapture());
        cvCvtColor(f, f, CV_BGR2HSV);
        CvScalar scal;
        scal = cvGet2D(f, y, x);

        cam->setHtab(cam->getInd(),scal.val[0]);
        cam->setStab(cam->getInd(),scal.val[1]);
        cam->setVtab(cam->getInd(),scal.val[2]);

        cout << "H-S-V" << "    " << scal.val[0] << "-" << scal.val[1] << "-" << scal.val[2] << endl;
    }

}

/****************************************************
*****           DEBUT DE L'OBJET                *****
****************************************************/
UTCamera::UTCamera(int camera) {

    // Nommage automatique de l'objet
    if(camera==CAMERA_AVA) { nameCam="camera avant";  }
    if(camera==CAMERA_ARR) { nameCam="camera arriere";}

    // Affectation à une caméra
    ptrCamera = cvCreateCameraCapture(camera);

    frame     = cvQueryFrame( ptrCamera );
    if( !frame ) return;

    // Dimension des images de la caméra
    width = frame->width;
    height= frame->height;

    // Initialisation du tableau masque
    imgTab = (unsigned char**)malloc(sizeof(unsigned char*)*width);
    for(int x=0 ; x<width ; x++){
        imgTab[x] = (unsigned char*)malloc(sizeof(unsigned char)*height);
    }

    // Chargement de la config
    loadConfig();
}


// Methode de calibrage
void UTCamera::calibrage()
{
    // Création des fenetres de seuillages
    cvNamedWindow( nameCam.c_str(), CV_WINDOW_AUTOSIZE );

    cvNamedWindow( "parametre 1"    , CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "parametre 2"    , CV_WINDOW_AUTOSIZE );
    cvCreateTrackbar("num : HSV"    , "parametre 1", &ind    , 6  , checkCouleur);

    cvMoveWindow("parametre 1",0,0);
    cvMoveWindow("parametre 2",300,0);

    cvCreateTrackbar("NOIR  : tolerance H" , "parametre 1", Htol+NOIR, 255, NULL);
    cvCreateTrackbar("NOIR  : tolerance S" , "parametre 1", Stol+NOIR, 255, NULL);
    cvCreateTrackbar("NOIR  : tolerance V" , "parametre 1", Vtol+NOIR, 255, NULL);
    cvCreateTrackbar("JAUNE : tolerance H", "parametre 1", Htol+JAUNE, 255, NULL);
    cvCreateTrackbar("JAUNE : tolerance S", "parametre 1", Stol+JAUNE, 255, NULL);
    cvCreateTrackbar("JAUNE : tolerance V", "parametre 1", Vtol+JAUNE, 255, NULL);
    cvCreateTrackbar("ROUGE : tolerance H", "parametre 1", Htol+ROUGE, 255, NULL);
    cvCreateTrackbar("ROUGE : tolerance S", "parametre 1", Stol+ROUGE, 255, NULL);
    cvCreateTrackbar("ROUGE : tolerance V", "parametre 1", Vtol+ROUGE, 255, NULL);
    cvCreateTrackbar("VERT  : tolerance H", "parametre 2", Htol+VERT, 255, NULL);
    cvCreateTrackbar("VERT  : tolerance S", "parametre 2", Stol+VERT, 255, NULL);
    cvCreateTrackbar("VERT  : tolerance V", "parametre 2", Vtol+VERT, 255, NULL);
    cvCreateTrackbar("BLEU  : tolerance H", "parametre 2", Htol+BLEU, 255, NULL);
    cvCreateTrackbar("BLEU  : tolerance S", "parametre 2", Stol+BLEU, 255, NULL);
    cvCreateTrackbar("BLEU  : tolerance V", "parametre 2", Vtol+BLEU, 255, NULL);
    cvCreateTrackbar("BLANC : tolerance H", "parametre 2", Htol+BLANC, 255, NULL);
    cvCreateTrackbar("BLANC : tolerance S", "parametre 2", Stol+BLANC, 255, NULL);
    cvCreateTrackbar("BLANC : tolerance V", "parametre 2", Vtol+BLANC, 255, NULL);

    cvSetMouseCallback(nameCam.c_str(), souris, this);

    // Boucle principale
    while(1) {
        frame = cvQueryFrame( ptrCamera );
        if( !frame ) break;


        maskApplication();
        afficherMask();
        RegGApplication();
        afficherRegi();

        cvCreateTrackbar("NOIR  : tolerance H" , "parametre 1", Htol+NOIR, 255, NULL);
        cvCreateTrackbar("NOIR  : tolerance S" , "parametre 1", Stol+NOIR, 255, NULL);
        cvCreateTrackbar("NOIR  : tolerance V" , "parametre 1", Vtol+NOIR, 255, NULL);
        cvCreateTrackbar("JAUNE : tolerance H", "parametre 1", Htol+JAUNE, 255, NULL);
        cvCreateTrackbar("JAUNE : tolerance S", "parametre 1", Stol+JAUNE, 255, NULL);
        cvCreateTrackbar("JAUNE : tolerance V", "parametre 1", Vtol+JAUNE, 255, NULL);
        cvCreateTrackbar("ROUGE : tolerance H", "parametre 1", Htol+ROUGE, 255, NULL);
        cvCreateTrackbar("ROUGE : tolerance S", "parametre 1", Stol+ROUGE, 255, NULL);
        cvCreateTrackbar("ROUGE : tolerance V", "parametre 1", Vtol+ROUGE, 255, NULL);
        cvCreateTrackbar("VERT  : tolerance H", "parametre 2", Htol+VERT,  255, NULL);
        cvCreateTrackbar("VERT  : tolerance S", "parametre 2", Stol+VERT,  255, NULL);
        cvCreateTrackbar("VERT  : tolerance V", "parametre 2", Vtol+VERT,  255, NULL);
        cvCreateTrackbar("BLEU  : tolerance H", "parametre 2", Htol+BLEU,  255, NULL);
        cvCreateTrackbar("BLEU  : tolerance S", "parametre 2", Stol+BLEU,  255, NULL);
        cvCreateTrackbar("BLEU  : tolerance V", "parametre 2", Vtol+BLEU,  255, NULL);
        cvCreateTrackbar("BLANC : tolerance H", "parametre 2", Htol+BLANC, 255, NULL);
        cvCreateTrackbar("BLANC : tolerance S", "parametre 2", Stol+BLANC, 255, NULL);
        cvCreateTrackbar("BLANC : tolerance V", "parametre 2", Vtol+BLANC, 255, NULL);
        cvShowImage( nameCam.c_str(), frame );

        char c = cvWaitKey(10);
        if( c == 'q' ) break;
    }

    // Sauvegarde de fin
    saveConfig();

    // Destruction
    cvDestroyWindow("parametre 1");
    cvDestroyWindow("parametre 2");
    cvDestroyWindow(nameCam.c_str());
}


/****************************************************
*****          METHODES DE TRAITEMENTS          *****
****************************************************/
// Application du filtre de couleur
void UTCamera::maskApplication()
{

    IplImage * hsv = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
    cvCvtColor(frame, hsv, CV_BGR2HSV);

    int h,s,v;

    for(int x=0; x<width; x++) {
        for(int y=0; y<height; y++) {

            h = CV_IMAGE_ELEM( hsv, uchar, y, ( x * 3 ) );
            s = CV_IMAGE_ELEM( hsv, uchar, y, ( x * 3 ) +1);
            v = CV_IMAGE_ELEM( hsv, uchar, y, ( x * 3 ) +2);

            if( seuil( h, Htab[BLEU], Htol[BLEU])   &&
                seuil( s, Stab[BLEU], Stol[BLEU])   &&
                seuil( v, Vtab[BLEU], Vtol[BLEU]))
                {
                    imgTab[x][y]=BLEU;
                }
       else if( seuil( h, Htab[ROUGE], Htol[ROUGE]) &&
                seuil( s, Stab[ROUGE], Stol[ROUGE]) &&
                seuil( v, Vtab[ROUGE], Vtol[ROUGE]))
                {
                    imgTab[x][y]=ROUGE;
                }
       else if( seuil( h, Htab[VERT], Htol[VERT])   &&
                seuil( s, Stab[VERT], Stol[VERT])   &&
                seuil( v, Vtab[VERT], Vtol[VERT]))
                {
                    imgTab[x][y]=VERT;
                }
       else if( seuil( h, Htab[JAUNE], Htol[JAUNE]) &&
                seuil( s, Stab[JAUNE], Stol[JAUNE]) &&
                seuil( v, Vtab[JAUNE], Vtol[JAUNE]))
                {
                    imgTab[x][y]=JAUNE;
                }
       else if( seuil( h, Htab[NOIR], Htol[NOIR])   &&
                seuil( s, Stab[NOIR], Stol[NOIR])   &&
                seuil( v, Vtab[NOIR], Vtol[NOIR]))
                {
                    imgTab[x][y]=NOIR;
                }
       else if( seuil( h, Htab[BLANC], Htol[BLANC]) &&
                seuil( s, Stab[BLANC], Stol[BLANC]) &&
                seuil( v, Vtab[BLANC], Vtol[BLANC]))
                {
                    imgTab[x][y]=BLANC;
                }
          else
                {
                    imgTab[x][y]=UNDEFINE;
                }
        }
    }
    cvReleaseImage(&hsv);
}

// Fonction pour étendre une zone
void extendRegion(region* z, int width, int height, unsigned char** imgTab, int x, int y)
{
		(z->nbPixel)++;
		imgTab[x][y] = UNDEFINE;

		if( y < z->max[HAUT     ].y ) { z->max[HAUT     ] = cvPoint(x,y); }
		if( y > z->max[BAS      ].y ) { z->max[BAS      ] = cvPoint(x,y); }
		if( x > z->max[DROITE   ].x ) { z->max[DROITE   ] = cvPoint(x,y); }
		if( x < z->max[GAUCHE   ].x ) { z->max[GAUCHE   ] = cvPoint(x,y); }

		if( (x+1<height)    && (imgTab[x+1][y]==z->color) ){
			extendRegion(z, width, height, imgTab, x+1, y);
		}
		if( (x-1>=0)        && (imgTab[x-1][y]==z->color) ){
			extendRegion(z, width, height, imgTab, x-1, y);
		}
		if( (y+1<width)     && (imgTab[x][y+1]==z->color) ){
			extendRegion(z, width, height, imgTab, x, y+1);
		}
		if( (y-1>=0)        && (imgTab[x][y-1]==z->color) ){
			extendRegion(z, width, height, imgTab, x, y-1);
		}
}

// Methode pour appliquer le region growing et remplir la liste des regions de l'objet
void UTCamera::RegGApplication()
{

    listeR.destroy();
    region *r;

	for( int x=0 ; x<width ; x++ ){
		for( int y=0 ; y<height ; y++ ){

			if(imgTab[x][y]!=UNDEFINE)
			{
			    r = (region*)malloc(sizeof(region));

				r->max[HAUT  ] = cvPoint(x,y);
				r->max[BAS   ] = cvPoint(x,y);
				r->max[DROITE] = cvPoint(x,y);
				r->max[GAUCHE] = cvPoint(x,y);
				r->color       = imgTab[x][y];
				r->nbPixel     = 0;

				extendRegion(r, width, height, imgTab, x, y);

				if( (r->nbPixel > 1000) )
				{
				    listeR.add(r);
				}
				else
				{
                     free(r);
				}
			}
		}
	}
}

/****************************************************
*****             METHODES D'AFFICHAGE          *****
****************************************************/
void afficheRegion(IplImage* imgPtr, Region *z)
{
    if(z!=NULL){
        CvFont font;
        double hScale=0.5;
        double vScale=0.5;
        int    lineWidth=1;
        cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

        int affiche = FALSE;

        if(z->color==JAUNE){
            cvPutText (imgPtr,"PIONT",cvPoint( (z->max[DROITE].x+z->max[GAUCHE].x)/2,(z->max[HAUT].y+z->max[BAS].y)/2 ), &font, cvScalar(0,0,0));
            affiche = TRUE;
        }
        if(z->color==ROUGE){
            cvPutText (imgPtr,"zone rouge",cvPoint( (z->max[DROITE].x+z->max[GAUCHE].x)/2,(z->max[HAUT].y+z->max[BAS].y)/2 ), &font, cvScalar(0,0,0));
            affiche = TRUE;
        }
        if(z->color==BLEU){
            cvPutText (imgPtr,"zone bleu",cvPoint( (z->max[DROITE].x+z->max[GAUCHE].x)/2,(z->max[HAUT].y+z->max[BAS].y)/2 ), &font, cvScalar(0,0,0));
            affiche = TRUE;
        }
        if(z->color==VERT){
            cvPutText (imgPtr,"zone verte",cvPoint( (z->max[DROITE].x+z->max[GAUCHE].x)/2,(z->max[HAUT].y+z->max[BAS].y)/2 ), &font, cvScalar(0,0,0));
            affiche = TRUE;
        }

        if(affiche){
            cvLine(imgPtr, z->max[HAUT], z->max[DROITE], cvScalar(0,0,0), 3);
            cvLine(imgPtr, z->max[HAUT], z->max[GAUCHE], cvScalar(0,0,0), 3);
            cvLine(imgPtr, z->max[BAS ], z->max[DROITE], cvScalar(0,0,0), 3);
            cvLine(imgPtr, z->max[BAS ], z->max[GAUCHE], cvScalar(0,0,0), 3);
        }
    }
}



// Fonction qui affiche les régions sur l'image principale
void UTCamera::afficherRegi()
{
    listeR.iniPtr();

    while(listeR.getEle()!=NULL){
        afficheRegion(frame, listeR.getEle());
        listeR.ptrSuiv();
    }
}


// Methodes pour afficher l'image après application du masque
void UTCamera::afficherMask()
{
    cvNamedWindow( "Mask", CV_WINDOW_AUTOSIZE );

    IplImage * imgPrint = cvCreateImage(cvSize(width, height), frame->depth, frame->nChannels);
    for(int x=0; x < width; x++) {
        for(int y=0; y < height; y++) {

            if(imgTab[x][y]==JAUNE){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 255;
            }
            if(imgTab[x][y]==BLEU){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 0;
            }
            if(imgTab[x][y]==ROUGE){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 255;
            }
            if(imgTab[x][y]==VERT){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 0;
            }
            if(imgTab[x][y]==BLANC){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 255;
            }
            if(imgTab[x][y]==NOIR){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 0;
            }
            if(imgTab[x][y]==UNDEFINE){
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 )    ) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, y, ( x * 3 ) + 2) = 255;
            }

        }
    }

    cvShowImage("Mask", imgPrint);
    cvReleaseData(imgPrint);
}


/****************************************************
*****     METHODES DE GESTION DES CONFIG        *****
****************************************************/

// Permet de sauvgarder les valeurs de configuration
void UTCamera::saveConfig()
{
    FILE* fichierConfig;

    fichierConfig = fopen("UTConfig.txt","w+");
    if(fichierConfig==NULL){return;}

    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",Htab[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",Htol[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",Stab[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",Stol[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",Vtab[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",Vtol[i]);}
    fprintf(fichierConfig,"\n");

    fclose(fichierConfig);
}

// Permet de charger le fichier de configuration des caméras
void UTCamera::loadConfig()
{
    FILE* fichierConfig;

    fichierConfig = fopen("UTConfig.txt","r");
    if(fichierConfig==NULL){return;}

    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Htab+i));}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Htol+i));}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Stab+i));}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Stol+i));}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Vtab+i));}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Vtol+i));}
    fprintf(fichierConfig,"\n");

    fclose(fichierConfig);
}

/****************************************************
*****            FIN DE L'OBJET                 *****
****************************************************/
// ---
UTCamera::~UTCamera() {
    cvReleaseCapture( &ptrCamera );
    cvDestroyWindow("Mask");
}

