    /*** INCLUDE ***/
#include "comManager.hpp"
#include <sstream>

IplImage* src;
pthread_mutex_t mu  = PTHREAD_MUTEX_INITIALIZER;

regionLister*   listTmp;


int s2i(const string & s)
{
	int a;
	istringstream iss(s);
	iss >> a;
	return a;
}

void parse(const string & s, int *id_from, int *id_msg, int *id_cmd)
{
	size_t index1 = s.find_first_not_of("0123456789",0);

	*id_from = s2i(s.substr(0,index1));

	size_t index2 = s.find_first_not_of("0123456789",index1+1);

	*id_msg = s2i(s.substr(index1+1,index2));

	*id_cmd = s2i(s.substr(index2+1));
}


/**
 *
 */
void sendData(regionLister* liste)
{
    printf("[");
    int nb = 0;
    liste->iniPtr();
    while(liste->getEle()!=NULL){
        if(liste->getEle()->color>=3){
            if(nb){
                printf(",");
            }
            printf("[1,%d,%d]",(int)liste->getEle()->Yprime,(int)liste->getEle()->Xprime);
            nb++;
        }
        liste->ptrSuiv();
    }

    printf("]");
}

/**
 *
 */
void moyenneArriere()
{


}


/**
    Fonction principale de traitement
*/
void modeStationnaire()
{
        // Declaration
    //Fps timeCounter;
    int  on = 1;
    pthread_t grabF;
    pthread_t grabB;
	int id_from, id_msg, ordre = 100;

    cvNamedWindow("T",CV_WINDOW_AUTOSIZE);
    std::cerr << std::endl << std::endl;

        // Lancement des threads
    if(captureCameraF!=NULL){
        pthread_create ( &grabF, NULL, grabCamF, NULL );
    }
    if(captureCameraB!=NULL){
        pthread_create ( &grabB, NULL, grabCamB, NULL );
    }

    string s;
        // Boucle principale
    while(on){
        cin >> s;
        parse(s,&id_from, &id_msg, &ordre);
        cout << -1 << '.' << id_msg << '.';
        //cout << id_from << id_msg << ordre;
        //scanf("%d.%d.%d",&id_from, &id_msg, &ordre);
        //fflush(stdin);

        //printf("-1.%d.[]",id_msg);

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
            case 64:{
                    pthread_mutex_lock (& mu);
                    imgBas=cvQueryFrame(captureCameraF);
                    pthread_mutex_unlock (& mu);

                    listFinal->iniPtr();
                    while(listFinal->getEle()!=NULL){
                        listFinal->getEle()->color=5;
                        listFinal->ptrSuiv();
                    }

                    binarisation();
                    regionGrowing();
                    listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                    sendData(listFinal);

                    //afficheListeRegion(listFinal, imgBas);
                    //cvShowImage("T",imgBas);

                    cvWaitKey(10);
            break;}

            // --- ScanArriére ---
            case 65:{

                    pthread_mutex_lock (& mu);
                    imgBas=cvQueryFrame(captureCameraB);
                    pthread_mutex_unlock (& mu);

                    listFinal->iniPtr();
                    while(listFinal->getEle()!=NULL){
                        listFinal->getEle()->color=5;
                        listFinal->ptrSuiv();
                    }

                    binarisation();
                    regionGrowing();
                    listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                    sendData(listFinal);


                    //afficheListeRegion(listFinal, imgBas);
                    //cvShowImage("T",imgBas);

                    cvWaitKey(10);
            break;}

#define SXL 20
            case 62:{

                for(int tmp=0; tmp<5; tmp++)
                {
                    pthread_mutex_lock (& mu);
                    imgBas=cvQueryFrame(captureCameraF);
                    pthread_mutex_unlock (& mu);

                    binarisation();
                    regionGrowing();

                    if(tmp==0){
                        listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                        listFinal->iniPtr();
                        while(listFinal->getEle()!=NULL){
                            listFinal->getEle()->color=0;
                            listFinal->ptrSuiv();
                        }
                    }else{
                        listTmp = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                        listFinal->iniPtr();
                        while(listFinal->getEle()!=NULL){
                            listTmp->iniPtr();
                            while(listTmp->getEle()!=NULL){

                                if
                                (
                                    listTmp->getEle()->Xprime - SXL < listFinal->getEle()->Xprime &&
                                    listTmp->getEle()->Xprime + SXL > listFinal->getEle()->Xprime &&
                                    listTmp->getEle()->Yprime - SXL < listFinal->getEle()->Yprime &&
                                    listTmp->getEle()->Yprime + SXL > listFinal->getEle()->Yprime
                                )
                                {
                                    listFinal->getEle()->color++;
                                }
                                listTmp->ptrSuiv();
                            }
                            listFinal->ptrSuiv();
                        }
                    }
                }

                sendData(listFinal);

            break;}

            case 63:{

                for(int tmp=0; tmp<5; tmp++)
                {
                    pthread_mutex_lock (& mu);
                    imgBas=cvQueryFrame(captureCameraB);
                    pthread_mutex_unlock (& mu);

                    binarisation();
                    regionGrowing();

                    if(tmp==0){
                        listFinal = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                        listFinal->iniPtr();
                        while(listFinal->getEle()!=NULL){
                            listFinal->getEle()->color=0;
                            listFinal->ptrSuiv();
                        }
                    }else{
                        listTmp = analyseListeRegion(listBeforeTrait, IMG_WIDTH, IMG_HEIGHT);
                        listFinal->iniPtr();
                        while(listFinal->getEle()!=NULL){
                            listTmp->iniPtr();
                            while(listTmp->getEle()!=NULL){

                                if
                                (
                                    listTmp->getEle()->Xprime - SXL < listFinal->getEle()->Xprime &&
                                    listTmp->getEle()->Xprime + SXL > listFinal->getEle()->Xprime &&
                                    listTmp->getEle()->Yprime - SXL < listFinal->getEle()->Yprime &&
                                    listTmp->getEle()->Yprime + SXL > listFinal->getEle()->Yprime
                                )
                                {
                                    listFinal->getEle()->color++;
                                }
                                listTmp->ptrSuiv();
                            }
                            listFinal->ptrSuiv();
                        }
                    }
                }

                sendData(listFinal);

            break;}

            // --- Arrét du programme ---
            case 13:{
                on = 0;
            break;}
            case 69:{
                on = 0;
            break;}
        }

        // *****
        cout << endl;
        fflush(stdout);
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
    return data;
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
    return data;
}
