    /*** INCLUDE ***/
#include "comManager.hpp"
#include <sstream>

IplImage* src;
pthread_mutex_t mu  = PTHREAD_MUTEX_INITIALIZER;


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
        if(nb){
            printf(",");
        }
        printf("[p,%ld,%ld]",(int)liste->getEle()->Yprime,(-1)*(int)liste->getEle()->Xprime);
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
