/**
 * \file loadCamera.cpp
 * \brief Algorithmes de reconnaissance des caméras
 * \version 1
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
#include "loadCamera.hpp"

/**
 * \fn void identificationCamera()
 * \brief fonction pour identifier les indexs des caméras
 */
void identificationCamera()
{
        // Déclaration et initialisation
    int index1=-1, index2=-1, nb;
    int usb1=-1, usb2=-1;
    char serial1[20], serial2[20];

    captureCameraF = NULL;
    captureCameraB = NULL;

        // Récupération des indexs videos
    checkIndexValide(&index1, &index2, &nb);
    if(nb==0){printf("Pas de cameras connectées\n");exit(EXIT_FAILURE);}

        // Identification des caméras sur les ports usb
    loadUsbVision(&usb1, serial1, &usb2, serial2);
    if(usb1==-1){printf("Pas de cameras Logitech connectées\n");exit(EXIT_FAILURE);}


    printf("index 1 : %d  --- index 2 : %d \n",index1,index2);
    printf("Serial 1, %s usb %d  --- Serial 2, %s usb %d \n",serial1,usb1,serial2,usb2);

// --------------

    if(nb==1){
        printf("Une Camera : ");
        if(!strcmp(serial1,SERIAL_CAM_F)){
            printf("Camera Avant - index %d",index1);
            captureCameraF = cvCaptureFromCAM( index1 );
        }
        else{
            printf("Camera Arriere - index %d",index1);
            captureCameraB = cvCaptureFromCAM( index1 );
        }
    }

// --------------

    if(nb==2){

        printf("Deux Cameras : ");
        if(!strcmp(serial1,SERIAL_CAM_F)){

            if(usb1>usb2){
                printf("Camera Avant - index %d "  ,index1 );
                captureCameraF = cvCaptureFromCAM(  index1 );
                printf("Camera Arriere - index %d ",index2 );
                captureCameraB = cvCaptureFromCAM(  index2 );}

            else{
                printf("Camera Avant - index %d "  ,index2 );
                captureCameraF = cvCaptureFromCAM(  index2 );
                printf("Camera Arriere - index %d ",index1 );
                captureCameraB = cvCaptureFromCAM(  index1 );}

        }
        else{

            if(usb1<usb2){
                printf("Camera Avant - index %d "  ,index2 );
                captureCameraF = cvCaptureFromCAM(  index2 );
                printf("Camera Arriere - index %d ",index1 );
                captureCameraB = cvCaptureFromCAM(  index1 );}

            else{
                printf("Camera Avant - index %d "  ,index1 );
                captureCameraF = cvCaptureFromCAM(  index1 );
                printf("Camera Arriere - index %d ",index2 );
                captureCameraB = cvCaptureFromCAM(  index2 );}

        }

    }
}


/**
 * \fn void checkIndexValide(int *index1, int *index2, int *nb)
 * \brief fonction qui récupére les index valide de caméra
 */
void checkIndexValide(int *index1, int *index2, int *nb)
{
    FILE* f=NULL;
    *nb = 0;

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


/**
 * \fn void loadUsbVision(int* usb1, char* serial1, int* usb2, char* serial2)
 * \brief fonction qui récupére le serial des cam et le numéro des ports usb
 */
void loadUsbVision(int* usb1, char* serial1, int* usb2, char* serial2)
{
    system(MAIN_DIR"searchCameraOnUsb");
    FILE* f;
    f = fopen(MAIN_DIR"usbVision","r");
    if(!f){return;}

    fscanf(f,"%d", usb1);
    fscanf(f,"%s", serial1);
    fscanf(f,"%d", usb2);
    fscanf(f,"%s", serial2);

    fclose(f);
}




