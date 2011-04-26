/**
 * \file camCalibre.cpp
 * \brief Algorithmes pour calibrer les caméras
 * \version 1
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
#include "camCalibre.hpp"
#include "zonesAnalyser.hpp"

CvCapture*  tmpCapture;
int Xindex = JAUNE;

/**
    Fonction permttant la calibration des cameras
*/
void calibrationManuel()
{
    // Déclaration
    cvNamedWindow("CalibrationManuel",CV_WINDOW_NORMAL);
    cvNamedWindow("HSV"              ,CV_WINDOW_NORMAL);
    cvNamedWindow("VisionBinariser"  ,CV_WINDOW_NORMAL);
    cvNamedWindow("Parametres"       ,CV_WINDOW_NORMAL);

    // Placement des fenetres
    cvResizeWindow("CalibrationManuel",400  ,300);
    cvResizeWindow("HSV"              ,400  ,300);
    cvResizeWindow("VisionBinariser"  ,400  ,300);
    cvResizeWindow("Parametres"       ,800  ,350);

    cvMoveWindow("CalibrationManuel",0  ,0);
    cvMoveWindow("HSV"              ,400,0);
    cvMoveWindow("VisionBinariser"  ,800,0);
    cvMoveWindow("Parametres"       ,0,400);

    cvSetMouseCallback("CalibrationManuel", souris, NULL);

    // Choix de la camera
    if(captureCameraF!=NULL){tmpCapture=captureCameraF;}
    else                    {tmpCapture=captureCameraB;}

    int r,g,b;

    while(1){

        //*****
        // Calculs

            cvGrabFrame(tmpCapture);
            imgBas=cvQueryFrame(tmpCapture);

            binarisation();
            binarisation2();
            regionGrowing();
            listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
            afficheListeRegion(listFinal, imgBas);

                        // AfficvWaitKey(0);chage et TrackBar
            cvCreateTrackbar("JAUNE : H"           , "Parametres", (_H+JAUNE), 255, NULL);
            cvCreateTrackbar("JAUNE : V"           , "Parametres", (_S+JAUNE), 255, NULL);
            cvCreateTrackbar("JAUNE : tolerance H" , "Parametres", (_Htol+JAUNE), 255, NULL);
            cvCreateTrackbar("JAUNE : tolerance V" , "Parametres", (_Stol+JAUNE), 255, NULL);

            cvShowImage("CalibrationManuel",imgBas);
            cvShowImage("HSV"              ,imgHsv);
            cvShowImage("VisionBinariser"  ,imgBin2);

        //*****

        char c = cvWaitKey(10);
        if( c == 'q' ) break;
    }

    saveConfig();

}


/**
    Fonction callback souris
*/
void souris(int event, int x, int y, int flags, void *param) {
    if(event == CV_EVENT_LBUTTONUP) {

        IplImage* f;
        //f=cvRetrieveFrame(tmpCapture);
        f=imgBas;

        cvCvtColor(f, f, CV_BGR2HSV);
        CvScalar scal;
        scal = cvGet2D(f, y, x);

        _H[Xindex] = scal.val[0];
        _S[Xindex] = scal.val[1];

        cout << "H " << scal.val[0] << " - V " << scal.val[1] << endl;
    }
}


/**
    Fonction de chargement du fichier de config
*/
void loadConfig()
{
    FILE* fichierConfig;

    fichierConfig = fopen(MAIN_DIR"UTConfig.txt","r");
    if(fichierConfig==NULL){return;}

    //    --- VALEUR DE H ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(_H+i));}
    fprintf(fichierConfig,"\n");

    if(_H[JAUNE]==0){_H[JAUNE]=30;}

    //    --- TOLERANCE DE H ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(_Htol+i));}
    fprintf(fichierConfig,"\n");

    if(_Htol[JAUNE]==0){_Htol[JAUNE]=10;}

    //    --- VALEUR DE S ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(_S+i));}
    fprintf(fichierConfig,"\n");

    if(_S[JAUNE]==0){_S[JAUNE]=10;}

    //    --- TOLERANCE DE S ---
    for(int i=0; i<NB_COLOR; i++){fscanf(fichierConfig,"%d   ",(_Stol+i));}
    fprintf(fichierConfig,"\n");

    if(_Stol[JAUNE]==0){_Stol[JAUNE]=60;}

    fclose(fichierConfig);
}

/**
    Fonction de sauvgarde du fichier de config
*/
void saveConfig()
{
    FILE* fichierConfig;

    fichierConfig = fopen(MAIN_DIR"UTConfig.txt","w+");
    if(fichierConfig==NULL){return;}

    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",_H[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",_Htol[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",_S[i]);}
    fprintf(fichierConfig,"\n");
    for(int i=0; i<NB_COLOR; i++){fprintf(fichierConfig,"%d   ",_Stol[i]);}
    fprintf(fichierConfig,"\n");

    fclose(fichierConfig);
}


