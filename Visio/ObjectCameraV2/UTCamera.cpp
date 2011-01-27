/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr
    FRICOT      Pierre-Henri

    UTCoupe 2010-2011
    All rights reserved.

*   Objet qui permet de gérer les caméras du robot.

*   Permet de récupérer les informations importantes
*   de l'environnement grâce aux caméras.
*****************************************************
 **/
#include "UTCamera.hpp"
#include "UTCameraFct.hpp"
#include "Fps.hpp"


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

/****************************************************
*****           DEBUT DE L'OBJET                *****
****************************************************/

void UTCamera::generalRun()
{
    int ordre=0;
    while(1){
        if(idCamera==waitOrder()){
            int i=0;
            do{
                for(int y=0; y<40 ; y++){
                basFrame = cvQueryFrame( ptrCamera );
                if(!basFrame){break;}}
            }while( !basFrame );

            maskApplication(MASK_BINARISATION);
            reggApplication();
            analyseRegion();
            send(listeP);
        }
    }
}


// **************************************************
// ***************** CONSTRUCTEUR *******************
UTCamera::UTCamera(int camera, int wScreen, int hScreen)
{
    idCamera = camera;

    // Attribution automatique d'un nom à l'objet
    if(camera==CAMERA_AVA) { nameCam="camera avant  ";}
    if(camera==CAMERA_ARR) { nameCam="camera arriere";}

    // Affectation de la caméra
    ptrCamera = cvCaptureFromCAM( camera       );
    basFrame     = cvQueryFrame    ( ptrCamera );
    if( !basFrame ){ cout << "Erreur initialisation : " << nameCam << endl; return;}

    // Dimension des images de la caméra
    width = basFrame->width;
    height= basFrame->height;

    this->wScreen = wScreen;
    this->hScreen = hScreen;

    // Initialisation de la police d'écriture
    double hScale=0.5;
    double vScale=0.5;
    int    lineWidth=1;
    cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

    //
    listeR = new regionLister();
    listeP = new regionLister();

    // Réservation de la place pour data
    data = (threaddata**)malloc(sizeof(threaddata*)*NB_THREAD);
    for(int i=0 ; i<NB_THREAD ; i++){
        data[i] = (threaddata*)malloc(sizeof(threaddata));
    }

    for(int i=0 ; i<NB_THREAD ; i++){
        data[i]->nbRow = height/4;
        data[i]->firstRow = i * data[i]->nbRow;
        data[i]->Htab = Htab; data[i]->Htol = Htol;
        data[i]->Stab = Stab; data[i]->Stol = Stol;
        data[i]->Vtab = Vtab; data[i]->Vtol = Vtol;
        data[i]->width = width;
        data[i]->height = height;
    }

    // Initialisation des images
    binFrame = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U   , 1                  );
    colFrame = cvCreateImage(cvSize(width, height), basFrame->depth, basFrame->nChannels);
    hsvFrame = cvCreateImage(cvSize(width, height), basFrame->depth, basFrame->nChannels);

    // Chargement de la config
    loadConfig();
}

// **************************************************
// ***** Methode de calibrage avant utilisation *****
void UTCamera::calibrage()
{
    Fps fpsVisio;

    // Création des fenetres de seuillages
    cvNamedWindow( nameCam.c_str()  , CV_WINDOW_NORMAL );
    cvNamedWindow( "Mask Test"      , CV_WINDOW_NORMAL );
    cvNamedWindow( "parametre 1"    , CV_WINDOW_NORMAL );
    cvNamedWindow( "parametre 2"    , CV_WINDOW_NORMAL );
    cvResizeWindow(nameCam.c_str(), DIV2INF(wScreen),DIV2INF(hScreen));
    cvMoveWindow(  nameCam.c_str(), DIV2SUP(wScreen),DIV2SUP(hScreen));
    cvResizeWindow("Mask Test"    , DIV2INF(wScreen),DIV2SUP(hScreen));
    cvMoveWindow(  "Mask Test"    , DIV2SUP(wScreen),0               );
    cvResizeWindow("parametre 1"  , DIV2SUP(DIV2SUP(wScreen)),hScreen);
    cvMoveWindow(  "parametre 1"  , 0               ,0               );
    cvResizeWindow("parametre 2"  , DIV2INF(DIV2SUP(wScreen)),hScreen);
    cvMoveWindow(  "parametre 2"  , DIV2INF(DIV2SUP(wScreen)),0      );

    cvCreateTrackbar( TEXTE, "parametre 1" , &ind    , 6  , NULL);
    TRACKBARS

    cvSetMouseCallback(nameCam.c_str(), souris, this);

    // *********************
    while(1){

        fpsVisio.debutMesureTime();

            // --- Traitements ---
        basFrame = cvQueryFrame( ptrCamera );
        if( !basFrame ) break;
        maskApplication(MASK_COLOR);
        maskApplication(MASK_BINARISATION);
        reggApplication();
        analyseRegion();
        afficherRegi();

        //cvLine(basFrame, cvPoint(0,400), cvPoint(width,400), cvScalar(0,0,0), 3);

        fpsVisio.finMesureTime();

            // --- Affichages ---
        cvShowImage( "Mask Test", colFrame );
        cvShowImage( nameCam.c_str(), basFrame );
        TRACKBARS

        //cout << fpsVisio.getMilli() << " ms" << endl;
        char c = cvWaitKey(10);
        if( c == 'q' ) break;
    }
    // *********************

    // Sauvegarde de fin
    saveConfig();

    // Destruction
    cvDestroyWindow("parametre 1");
    cvDestroyWindow("parametre 2");
    cvDestroyWindow("Mask Test  ");
    cvDestroyWindow(nameCam.c_str());
}


/****************************************************
*****          METHODES DE TRAITEMENTS          *****
****************************************************/

// **************************************************
// ********* Binarisation ou Colorisation ***********
void UTCamera::maskApplication(int mode)
{
    // Image temporaire convertie en hsv
    cvCvtColor(basFrame, hsvFrame, CV_BGR2HSV);

    for(int i=0 ; i<NB_THREAD ; i++){
        data[i]->src = hsvFrame;
        if(mode==MASK_BINARISATION){
            data[i]->dst = binFrame;
            pthread_create((th+i), NULL, binThread, (void*)*(data+i));
        }else{
            data[i]->dst = colFrame;
            pthread_create((th+i), NULL, colThread, (void*)*(data+i));
        }
    }

    for(int i=0 ; i<NB_THREAD ; i++){
        pthread_join (th[i], NULL);
    }
}

// **************************************************
// Methode pour appliquer le region growing et remplir la liste des regions de l'objet
void UTCamera::reggApplication()
{
    listeR->destroy();
    region *r; int c;

	for( int x=0 ; x<width ; x++ ){
		for( int y=0 ; y<height ; y++ ){
                c = CV_IMAGE_ELEM( binFrame, uchar, y, x );
			if(c==0)
			{
			    r = (region*)malloc(sizeof(region));

				r->max[HAUT  ] = cvPoint(x,y);
				r->max[BAS   ] = cvPoint(x,y);
				r->max[DROITE] = cvPoint(x,y);
				r->max[GAUCHE] = cvPoint(x,y);
				r->color       = c;
				r->nbPixel     = 0;

				extendRegion(r, width, height, binFrame, x, y, 0);

				if( (r->nbPixel > 1000) )
				{
				    listeR->add(r);
				}
				else
				{
                     free(r);
				}
			}
		}
	}
}

// **************************************************
// ********** Methode d'analyse des zones ***********
void UTCamera::analyseRegion()
{
    listeP = analyseListeRegion(listeR,width,height);
}


/****************************************************
*****             METHODES D'AFFICHAGE          *****
****************************************************/

// **************************************************
// Methode qui affiche les régions sur l'image principale
void UTCamera::afficherRegi()
{
    afficheListeRegion(listeP, basFrame);
}

// **************************************************
// ********  Methode pour l'enregistrement  *********
void UTCamera::record(char* nom, int mode){

    CvSize size = cvSize (
            (int)cvGetCaptureProperty( ptrCamera, CV_CAP_PROP_FRAME_WIDTH),
            (int)cvGetCaptureProperty( ptrCamera, CV_CAP_PROP_FRAME_HEIGHT) );
    CvVideoWriter *writer = cvCreateVideoWriter( nom, CV_FOURCC('M','J','P','G'), 20, size );
    if(writer == NULL){ cout << "RECORD PROBLEME !!!" << endl; return; }

    cvNamedWindow( nameCam.c_str()  , CV_WINDOW_AUTOSIZE );

    while(1){
        basFrame = cvQueryFrame( ptrCamera );
        if( !basFrame ) break;

        if(mode == VISION_NORMAL){
            cvWriteFrame( writer, basFrame );
            cvShowImage(nameCam.c_str(),basFrame);
        }


        char c = cvWaitKey(10);
        if( c == 'q' ) break;
    }

    cvReleaseVideoWriter( &writer );
    cvDestroyWindow(nameCam.c_str());
}

/****************************************************
*****     METHODES DE GESTION DES CONFIG        *****
****************************************************/
// **************************************************
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

// **************************************************
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
    if(Htab[VERT ]==0){Htab[VERT ]=120;}
    if(Htab[BLEU ]==0){Htab[BLEU ]=240;}
    if(Htab[JAUNE]==0){Htab[JAUNE]=32;}
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
    cvReleaseImage(&hsvFrame);
    cvReleaseCapture( &ptrCamera );
    free(data);
}

