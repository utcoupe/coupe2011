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
// ********************
// *** CONSTRUCTEUR ***
UTCamera::UTCamera(int camera, int wScreen, int hScreen) {

    // Nommage automatique de l'objet
    if(camera==CAMERA_AVA) { nameCam="camera avant";  }
    if(camera==CAMERA_ARR) { nameCam="camera arriere";}

    // Affectation à une caméra
    ptrCamera = cvCreateCameraCapture(camera);

    frame     = cvQueryFrame( ptrCamera );
    if( !frame ){ cout << "Erreur initialisation : " << nameCam << endl; return;}

    // Dimension des images de la caméra
    width = frame->width;
    height= frame->height;

    this->wScreen = wScreen;
    this->hScreen = hScreen;

    // Initialisation du tableau masque
    imgTab = (unsigned char**)malloc(sizeof(unsigned char*)*width);
    for(int x=0 ; x<width ; x++){
        imgTab[x] = (unsigned char*)malloc(sizeof(unsigned char)*height);
    }

    // Chargement de la config
    loadConfig();
}

// **********************************************
// *** Methode de calibrage avant utilisation ***
void UTCamera::calibrage()
{
    // Création des fenetres de seuillages
    cvNamedWindow( nameCam.c_str()  , CV_WINDOW_NORMAL );
    cvNamedWindow( "Mask Test"      , CV_WINDOW_NORMAL );

    cvNamedWindow( "parametre 1"    , CV_WINDOW_NORMAL );
    cvNamedWindow( "parametre 2"    , CV_WINDOW_NORMAL );

    cvResizeWindow(nameCam.c_str(), DIV2INF(wScreen),DIV2INF(hScreen));
    cvMoveWindow(  nameCam.c_str(), DIV2SUP(wScreen),DIV2SUP(hScreen));
    cvResizeWindow("Mask Test"    , DIV2INF(wScreen),DIV2SUP(hScreen));
    cvMoveWindow(  "Mask Test"    , DIV2SUP(wScreen),0               );
    afficherMask(  "Mask Test"                                       );
    cvResizeWindow("parametre 1"  , DIV2SUP(DIV2SUP(wScreen)),hScreen);
    cvMoveWindow(  "parametre 1"  , 0               ,0               );
    cvResizeWindow("parametre 2"  , DIV2INF(DIV2SUP(wScreen)),hScreen);
    cvMoveWindow(  "parametre 2"  , DIV2INF(DIV2SUP(wScreen)),0      );

    cvCreateTrackbar( TEXTE, "parametre 1" , &ind    , 6  , NULL);

    cvCreateTrackbar("NOIR  : tolerance H" , "parametre 1", Htol+NOIR, 255, NULL);
    cvCreateTrackbar("NOIR  : tolerance S" , "parametre 1", Stol+NOIR, 255, NULL);
    cvCreateTrackbar("NOIR  : tolerance V" , "parametre 1", Vtol+NOIR, 255, NULL);
    cvCreateTrackbar("ROUGE : tolerance H", "parametre 1", Htol+ROUGE, 255, NULL);
    cvCreateTrackbar("ROUGE : tolerance S", "parametre 1", Stol+ROUGE, 255, NULL);
    cvCreateTrackbar("ROUGE : tolerance V", "parametre 1", Vtol+ROUGE, 255, NULL);
    cvCreateTrackbar("VERT  : tolerance H", "parametre 2", Htol+VERT,  255, NULL);
    cvCreateTrackbar("VERT  : tolerance S", "parametre 2", Stol+VERT,  255, NULL);
    cvCreateTrackbar("VERT  : tolerance V", "parametre 2", Vtol+VERT,  255, NULL);
    cvCreateTrackbar("BLEU  : tolerance H", "parametre 2", Htol+BLEU,  255, NULL);
    cvCreateTrackbar("BLEU  : tolerance S", "parametre 2", Stol+BLEU,  255, NULL);
    cvCreateTrackbar("BLEU  : tolerance V", "parametre 2", Vtol+BLEU,  255, NULL);
    cvCreateTrackbar("JAUNE : tolerance H", "parametre 1", Htol+JAUNE, 255, NULL);
    cvCreateTrackbar("JAUNE : tolerance S", "parametre 1", Stol+JAUNE, 255, NULL);
    cvCreateTrackbar("JAUNE : tolerance V", "parametre 1", Vtol+JAUNE, 255, NULL);
    cvCreateTrackbar("BLANC : tolerance H", "parametre 2", Htol+BLANC, 255, NULL);
    cvCreateTrackbar("BLANC : tolerance S", "parametre 2", Stol+BLANC, 255, NULL);
    cvCreateTrackbar("BLANC : tolerance V", "parametre 2", Vtol+BLANC, 255, NULL);

    cvSetMouseCallback(nameCam.c_str(), souris, this);

    // Boucle principale
    while(1) {
        frame = cvQueryFrame( ptrCamera );
        if( !frame ) break;

        maskApplication();
        afficherMask("Mask Test");
        RegGApplication();
        afficherRegi();

        cvCreateTrackbar("NOIR  : tolerance H" , "parametre 1", Htol+NOIR, 255, NULL);
        cvCreateTrackbar("NOIR  : tolerance S" , "parametre 1", Stol+NOIR, 255, NULL);
        cvCreateTrackbar("NOIR  : tolerance V" , "parametre 1", Vtol+NOIR, 255, NULL);
        cvCreateTrackbar("ROUGE : tolerance H", "parametre 1", Htol+ROUGE, 255, NULL);
        cvCreateTrackbar("ROUGE : tolerance S", "parametre 1", Stol+ROUGE, 255, NULL);
        cvCreateTrackbar("ROUGE : tolerance V", "parametre 1", Vtol+ROUGE, 255, NULL);
        cvCreateTrackbar("VERT  : tolerance H", "parametre 2", Htol+VERT,  255, NULL);
        cvCreateTrackbar("VERT  : tolerance S", "parametre 2", Stol+VERT,  255, NULL);
        cvCreateTrackbar("VERT  : tolerance V", "parametre 2", Vtol+VERT,  255, NULL);
        cvCreateTrackbar("BLEU  : tolerance H", "parametre 2", Htol+BLEU,  255, NULL);
        cvCreateTrackbar("BLEU  : tolerance S", "parametre 2", Stol+BLEU,  255, NULL);
        cvCreateTrackbar("BLEU  : tolerance V", "parametre 2", Vtol+BLEU,  255, NULL);
        cvCreateTrackbar("JAUNE : tolerance H", "parametre 1", Htol+JAUNE, 255, NULL);
        cvCreateTrackbar("JAUNE : tolerance S", "parametre 1", Stol+JAUNE, 255, NULL);
        cvCreateTrackbar("JAUNE : tolerance V", "parametre 1", Vtol+JAUNE, 255, NULL);
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
    cvDestroyWindow("Mask Test");
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

            if(IS_COLOR(h,s,v,BLEU))
                {
                    imgTab[x][y]=BLEU;
                }
       else if(IS_COLOR(h,s,v,ROUGE))
                {
                    imgTab[x][y]=ROUGE;
                }
       else if(IS_COLOR(h,s,v,VERT))
                {
                    imgTab[x][y]=VERT;
                }
       else if(IS_COLOR(h,s,v,NOIR))
                {
                    imgTab[x][y]=NOIR;
                }
       else if(IS_COLOR(h,s,v,BLANC))
                {
                    imgTab[x][y]=BLANC;
                }
          else
                {
                    //if( SEUIL( h, Htab[JAUNE], Htol[JAUNE]) )
                    //{
                        imgTab[x][y]=JAUNE;
                    //}
                }
        }
    }
    cvReleaseImage(&hsv);
}

// Fonction pour étendre une zone
void extendRegion(region* z, int width, int height, unsigned char** imgTab, int x, int y, int stop)
{
    if(stop<STOP_REC){
		(z->nbPixel)++;
		imgTab[x][y] = UNDEFINE;

		if( y < z->max[HAUT     ].y ) { z->max[HAUT     ] = cvPoint(x,y); }
		if( y > z->max[BAS      ].y ) { z->max[BAS      ] = cvPoint(x,y); }
		if( x > z->max[DROITE   ].x ) { z->max[DROITE   ] = cvPoint(x,y); }
		if( x < z->max[GAUCHE   ].x ) { z->max[GAUCHE   ] = cvPoint(x,y); }

		if( (x+1<height)    && (imgTab[x+1][y]==z->color) ){
			extendRegion(z, width, height, imgTab, x+1, y, stop+1);
		}
		if( (x-1>=0)        && (imgTab[x-1][y]==z->color) ){
			extendRegion(z, width, height, imgTab, x-1, y, stop+1);
		}
		if( (y+1<width)     && (imgTab[x][y+1]==z->color) ){
			extendRegion(z, width, height, imgTab, x, y+1, stop+1);
		}
		if( (y-1>=0)        && (imgTab[x][y-1]==z->color) ){
			extendRegion(z, width, height, imgTab, x, y-1, stop+1);
		}
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

				extendRegion(r, width, height, imgTab, x, y, 0);

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

void UTCamera::afficherMask(){
    cvNamedWindow( "Mask", CV_WINDOW_AUTOSIZE );
    afficherMask("Mask");
}

// Methodes pour afficher l'image après application du masque
// Il faut créer une fenetre et passer en paramétre le nome de cette fenetre
void UTCamera::afficherMask(char* WindowMaskName)
{

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
    cvShowImage(WindowMaskName, imgPrint);
    cvReleaseData(imgPrint);
}


// Methode pour l'enregistrement
void UTCamera::record(int mode){

    CvSize size = cvSize (
            (int)cvGetCaptureProperty( ptrCamera, CV_CAP_PROP_FRAME_WIDTH),
            (int)cvGetCaptureProperty( ptrCamera, CV_CAP_PROP_FRAME_HEIGHT) );

    CvVideoWriter *writer = cvCreateVideoWriter(
            "out.avi", CV_FOURCC('M','J','P','G'), 22, size );

    if(writer == NULL){
            cout << "RECORD PROBLEME !!!" << endl; }

    if(mode == VISION_NORMAL){
        //cvWriteFrame( writer, frame );
    }
    if(mode == VISION_MASK){
        //cvWriteFrame( writer, frame );
    }
    if(mode == VISION_ZONE){
        //cvWriteFrame( writer, frame );
    }

    cvReleaseVideoWriter( &writer );

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

    //    --- VALEUR DE H ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Htab+i));}
    fprintf(fichierConfig,"\n");

    if(Htab[NOIR ]==0){Htab[NOIR ]=0;}
    if(Htab[ROUGE]==0){Htab[ROUGE]=0;}
    if(Htab[VERT ]==0){Htab[VERT ]=0;}
    if(Htab[BLEU ]==0){Htab[BLEU ]=0;}
    if(Htab[JAUNE]==0){Htab[JAUNE]=0;}
    if(Htab[BLANC]==0){Htab[BLANC]=0;}

    //    --- TOLERANCE DE H ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Htol+i));}
    fprintf(fichierConfig,"\n");

    if(Htol[NOIR ]==0){Htol[NOIR ]=10;}
    if(Htol[ROUGE]==0){Htol[ROUGE]=10;}
    if(Htol[VERT ]==0){Htol[VERT ]=10;}
    if(Htol[BLEU ]==0){Htol[BLEU ]=10;}
    if(Htol[JAUNE]==0){Htol[JAUNE]=10;}
    if(Htol[BLANC]==0){Htol[BLANC]=10;}

    //    --- VALEUR DE S ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Stab+i));}
    fprintf(fichierConfig,"\n");

    if(Stab[NOIR ]==0){Stab[NOIR ]=10;}
    if(Stab[ROUGE]==0){Stab[ROUGE]=10;}
    if(Stab[VERT ]==0){Stab[VERT ]=10;}
    if(Stab[BLEU ]==0){Stab[BLEU ]=10;}
    if(Stab[JAUNE]==0){Stab[JAUNE]=10;}
    if(Stab[BLANC]==0){Stab[BLANC]=10;}

    //    --- TOLERANCE DE S ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Stol+i));}
    fprintf(fichierConfig,"\n");

    if(Stol[NOIR ]==0){Stol[NOIR ]=60;}
    if(Stol[ROUGE]==0){Stol[ROUGE]=60;}
    if(Stol[VERT ]==0){Stol[VERT ]=60;}
    if(Stol[BLEU ]==0){Stol[BLEU ]=60;}
    if(Stol[JAUNE]==0){Stol[JAUNE]=60;}
    if(Stol[BLANC]==0){Stol[BLANC]=60;}

    //    --- VALEUR DE V ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Vtab+i));}
    fprintf(fichierConfig,"\n");

    if(Vtab[NOIR ]==0){Vtab[NOIR ]=10;}
    if(Vtab[ROUGE]==0){Vtab[ROUGE]=10;}
    if(Vtab[VERT ]==0){Vtab[VERT ]=10;}
    if(Vtab[BLEU ]==0){Vtab[BLEU ]=10;}
    if(Vtab[JAUNE]==0){Vtab[JAUNE]=10;}
    if(Vtab[BLANC]==0){Vtab[BLANC]=10;}

    //    --- TOLERANCE DE V ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(Vtol+i));}
    fprintf(fichierConfig,"\n");

    if(Vtol[NOIR ]==0){Vtol[NOIR ]=160;}
    if(Vtol[ROUGE]==0){Vtol[ROUGE]=160;}
    if(Vtol[VERT ]==0){Vtol[VERT ]=160;}
    if(Vtol[BLEU ]==0){Vtol[BLEU ]=160;}
    if(Vtol[JAUNE]==0){Vtol[JAUNE]=160;}
    if(Vtol[BLANC]==0){Vtol[BLANC]=160;}

    fclose(fichierConfig);
}

/****************************************************
*****            FIN DE L'OBJET                 *****
****************************************************/
// ---
UTCamera::~UTCamera() {
    cvReleaseCapture( &ptrCamera );
}

