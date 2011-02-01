/**
*****************************************************
    Copyright (c)
    RODRIGUEZ   Xavier          xavier.rodriguez@hds.utc.fr

    UTCoupe 2010-2011
    All rights reserved.

*   Permet l'interfaçage avec les consoles
*****************************************************
 **/

/****************************************************
*****           INCLUDES                        *****
****************************************************/
using namespace std;
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <sys/stat.h>

// OpenCV
#include <cv.h>
using namespace cv;
#include <highgui.h>

// Extern
#include "UTCamera.hpp"
#include "cam_regionAnalyser.hpp"
/****************************************************
*****           MACRO-DEFINITION                *****
****************************************************/

#define WIDTH_SCREEN    800
#define HEIGHT_SCREEN    800

#define SERIAL_CAM_F    "0A124C80"
#define SERIAL_CAM_B    "96124C80"

/****************************************************
*****           PROTOTYPES                      *****
****************************************************/
void loadUsbVision(int* usb1, char* serial1, int* usb2, char* serial2);
void loadIndexValide(int *index1, int *index2, int *nb);

// **************************************************
// *******************  MAIN  ***********************
int main(int argc, char* argv[])
{

    // Intialisation
    int idCamera    = -1;
    int modeCamera  = -1;
    int orientation = CAMERA_TEST;

    int usb1=-1,usb2=-1, index1=-1, index2=-1;
    int nbDeCamBranche=0;
    char serial1[20],serial2[20];

    loadUsbVision( &usb1, serial1, &usb2, serial2);
    if(usb1==-1){cout << 'c' << endl;exit(EXIT_FAILURE);}

    loadIndexValide(&index1, &index2, &nbDeCamBranche);
    if(nbDeCamBranche==0){cout << 'c' << endl;exit(EXIT_FAILURE);};

    /// Décodage des arguments
    int i=1;
    while(i<argc){

        // --- Selection de la camera avant ---
        if(!strcmp(argv[i],"-f"))
        {
                orientation = CAMERA_AVA;
            if(!strcmp(serial1,SERIAL_CAM_F)){
                if(nbDeCamBranche==1){ idCamera=index1;}
                else if(nbDeCamBranche==2)
                {
                    if(usb1<usb2){  idCamera=index1;}
                    else{           idCamera=index2;}
                }
            }
            else if(!strcmp(serial2,SERIAL_CAM_F)){
                if(nbDeCamBranche==1){ idCamera=index1;}
                else if(nbDeCamBranche==2)
                {
                    if(usb1<usb2){  idCamera=index2;}
                    else{           idCamera=index1;}
                }
            }
            else{
                cout << 'c' << endl;
                exit(EXIT_FAILURE);
            }
        // --- Selection de la camera arriére ---
        }
        else if(!strcmp(argv[i],"-b"))
        {
            orientation = CAMERA_ARR;
            if(!strcmp(serial1,SERIAL_CAM_B)){
                if(nbDeCamBranche==1){ idCamera=index1;}
                else if(nbDeCamBranche==2)
                {
                    if(usb1<usb2){  idCamera=index1;}
                    else{           idCamera=index2;}
                }
            }
            else if(!strcmp(serial2,SERIAL_CAM_B)){
                if(nbDeCamBranche==1){ idCamera=index1;}
                else if(nbDeCamBranche==2)
                {
                    if(usb1<usb2){  idCamera=index2;}
                    else{           idCamera=index1;}
                }
            }
            else{
                cout << 'c' << endl;
                exit(EXIT_FAILURE);
            }
        // --- Selection du mode ---
        }
        else if(!strcmp(argv[i],"-m")){
            i++;
            if(!strcmp(argv[i],"n")){
                modeCamera = 1;
            }
            else if(!strcmp(argv[i],"c")){
                modeCamera = 3;
            }


        // --- Erreur ---
        }else{
            cout << "Il faut lire la doc l'argument " << argv[i] << " n'est pas valide" << endl;
            exit(EXIT_FAILURE);
        }

        i++; // argument suivant
    }

    if(idCamera  ==-1){cout << 'c' << "Il manque le choix de la camera" << endl; exit(EXIT_FAILURE);}
    if(modeCamera==-1){cout << 'c' << "Il manque le mode de fonctionnement" << endl; exit(EXIT_FAILURE);}

    UTCamera *cam = new UTCamera(idCamera, orientation, WIDTH_SCREEN, HEIGHT_SCREEN);

    //cam->calibrage();
    //cam->record("tropClasse.avi",VISION_COLOR);

    if(modeCamera==1){cam->generalRun();}
    if(modeCamera==3){cam->calibrage();}

    exit(EXIT_SUCCESS);
}

/****************************************************
*****              FONCTIONS                    *****
****************************************************/


// **************************************************
void loadIndexValide(int *index1, int *index2, int *nb)
{
    FILE* f=NULL;

    //f = fopen("/dev/video0","r"); Camera de la webcam

    f = fopen("/dev/video1","r");
    if(f){*index1=1;*nb=1;fclose(f);}


    f = fopen("/dev/video2","r");
    if(f)
    {   if(*nb){*index2=2;*nb=2;fclose(f);return;}
        else{*index1=2;*nb=1;fclose(f);} }

    f = fopen("/dev/video3","r");
    if(f)
    {   if(*nb){*index2=3;*nb=2;fclose(f);return;}
        else{*index1=3;*nb=1;fclose(f);} }

    f = fopen("/dev/video4","r");
    if(f)
    {   if(*nb){*index2=4;*nb=2;fclose(f);return;}
        else{*index1=4;*nb=1;fclose(f);} }
}

// **************************************************
void loadUsbVision(int* usb1, char* serial1, int* usb2, char* serial2)
{
    system("./script.sh");
    FILE* f;
    f = fopen("usbVision","r");
    if(!f){return;}

    fscanf(f,"%d", usb1);
    fscanf(f,"%s", serial1);
    fscanf(f,"%d", usb2);
    fscanf(f,"%s", serial2);

    fclose(f);
}


