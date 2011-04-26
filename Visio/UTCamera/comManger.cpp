    /*** INCLUDE ***/
#include "comManager.hpp"

IplImage* src;
pthread_mutex_t mu  = PTHREAD_MUTEX_INITIALIZER;


/**
 *
 */
void sendData(regionLister* liste)
{
    printf("[");
    int nb = 0;
    liste->iniPtr();
    while(liste->getEle()!=NULL){
        if(nb){
            printf(",");
        }
        printf("[p,%ld,%ld]",(int)liste->getEle()->Yprime,(int)liste->getEle()->Xprime);
        nb++;
        liste->ptrSuiv();
    }

    printf("]");
}

/**
    Fonction principale de traitement
*/
void modeStationnaire()
{
        // Declaration
    Fps timeCounter;
    int ordre = 100;
    int  on = 1;
    pthread_t grabF;
    pthread_t grabB;

    cvNamedWindow("T",CV_WINDOW_AUTOSIZE);
    printf("\n\n");

        // Lancement des threads
    if(captureCameraF!=NULL){
        pthread_create ( &grabF, NULL, grabCamF, NULL );
    }
    if(captureCameraB!=NULL){
        pthread_create ( &grabB, NULL, grabCamB, NULL );
    }

        // Boucle principale
    while(on){
        scanf("%d",&ordre);
        fflush(stdin);
        // *****

        switch(ordre){

            // --- Identification ---
            case 0:{
                    printf("cam");
            break;}

            // --- Ping ---
            case 1:{
                    printf("%s","Pong");
            break;}

            // --- ScanAvant ---
            case 62:{
                    pthread_mutex_lock (& mu);
                    imgBas=cvQueryFrame(captureCameraF);
                    pthread_mutex_unlock (& mu);

                    binarisation();
                    regionGrowing();
                    listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                    sendData(listFinal);

                    afficheListeRegion(listFinal, imgBas);
                    //cvShowImage("T",imgBas);

                    cvWaitKey(10);
            break;}

            // --- ScanArriére ---
            case 63:{
                    pthread_mutex_lock (& mu);
                    imgBas=cvQueryFrame(captureCameraB);
                    pthread_mutex_unlock (& mu);

                    binarisation();
                    regionGrowing();
                    listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                    sendData(listFinal);

                    afficheListeRegion(listFinal, imgBas);
                    //cvShowImage("T",imgBas);

                    cvWaitKey(10);
            break;}

            // --- Arrét du programme ---
            case 69:{
                on = 0;
            break;}
        }

        // *****

    }
}



/**
    Fonction toujours graber la derniére image
*/
void* grabCamF(void* data)
{
    while(1){
        pthread_mutex_lock (& mu);
        cvGrabFrame( captureCameraF );
        pthread_mutex_unlock (& mu);
        cvWaitKey(10);
    }
}

/**
    Fonction toujours graber la derniére image
*/
void* grabCamB(void* data)
{
    while(1){
        pthread_mutex_lock (& mu);
        cvGrabFrame( captureCameraB );
        pthread_mutex_unlock (& mu);
        cvWaitKey(10);
    }
}
