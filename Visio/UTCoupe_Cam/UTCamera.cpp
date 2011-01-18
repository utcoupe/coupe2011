#include "UTCamera.hpp"

void checkCouleur(int valeur) {
    switch(valeur) {
    case UNDEFINE:
        cout<<"UNDEFINED"<<endl;
        break;
    case NOIR:
        cout<<"NOIR"<<endl;
        break;
    case ROUGE:
        cout<<"ROUGE"<<endl;
        break;
    case VERT:
        cout<<"VERT"<<endl;
        break;
    case BLEU:
        cout<<"BLEU"<<endl;
        break;
    case JAUNE:
        cout<<"JAUNE"<<endl;
        break;
    case BLANC:
        cout<<"BLANC"<<endl;
        break;
    }
}

void souris(int event, int x, int y, int flags, void *param) {

    if(event == CV_EVENT_LBUTTONUP) {
        IplImage* f;
        UTCam* cam = (UTCam*)param;

        f=cvRetrieveFrame(cam->getCapture());
        cvCvtColor(f, f, CV_BGR2HSV);
        CvScalar scal;
        scal = cvGet2D(f, y, x);

        cam->setHtab(cam->getInd(),scal.val[0]);
        cam->setStab(cam->getInd(),scal.val[1]);
        cam->setVtab(cam->getInd(),scal.val[2]);
    }

}


UTCam::UTCam(int camera) {

    if(camera==CAMERA_AVA) {
        nameCam="camera avant";
    }
    if(camera==CAMERA_ARR) {
        nameCam="camera arriere";
    }

    ptrCamera = cvCreateCameraCapture(camera);

    frame = cvQueryFrame( ptrCamera );
    if( !frame ) return;

    width = frame->width;
    height= frame->height;

    imgTab = (unsigned char**)malloc(sizeof(unsigned char*)*height);
    for(int i=0 ; i<height ; i++){
        imgTab[i] = (unsigned char*)malloc(sizeof(unsigned char)*width);
    }

    Htab[JAUNE]=32;
    Htol[5]=10;
    Htab[1]=0;

    Htab[2]=0;

    Stol[5]=60;
    Vtol[5]=120;
}


void UTCam::maskApplication()
{

    IplImage * hsv = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
    cvCvtColor(frame, hsv, CV_BGR2HSV);
    CvScalar scal;
    int width=frame->width;
    int height=frame->height;
    for(int x=0; x < height; x++) {
        for(int y=0; y < width; y++) {

            if( seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) ), Htab[NOIR], Htol[NOIR])   &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +1), Stab[NOIR], Stol[NOIR]) &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +2), Vtab[NOIR], Vtol[NOIR]))
                {
                    imgTab[x][y]=NOIR;
                }
       else if( seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) ), Htab[JAUNE], Htol[JAUNE])  &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +1), Stab[JAUNE], Stol[JAUNE])&&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +2), Vtab[JAUNE], Vtol[JAUNE]))
                {
                    imgTab[x][y]=JAUNE;
                }
       else if( seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) ), Htab[BLEU], Htol[BLEU])  &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +1), Stab[BLEU], Stol[BLEU])&&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +2), Vtab[BLEU], Vtol[BLEU]))
                {
                    imgTab[x][y]=BLEU;
                }
       else if( seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) ), Htab[ROUGE], Htol[ROUGE])  &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +1), Stab[ROUGE], Stol[ROUGE])&&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +2), Vtab[ROUGE], Vtol[ROUGE]))
                {
                    imgTab[x][y]=ROUGE;
                }
       else if( seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) ), Htab[VERT], Htol[VERT])  &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +1), Stab[VERT], Stol[VERT])&&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +2), Vtab[VERT], Vtol[VERT]))
                {
                    imgTab[x][y]=VERT;
                }
       else if( seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) ), Htab[BLANC], Htol[BLANC])  &&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +1), Stab[BLANC], Stol[BLANC])&&
                seuil( CV_IMAGE_ELEM( hsv, uchar, x, ( y * 3 ) +2), Vtab[BLANC], Vtol[BLANC]))
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


void UTCam::afficherMask()
{
    cvNamedWindow( "Mask", CV_WINDOW_AUTOSIZE );

    IplImage * imgPrint = cvCreateImage(cvSize(width, height), frame->depth, frame->nChannels);

    for(int x=0; x < height; x++) {
        for(int y=0; y < width; y++) {

            if(imgTab[x][y]==JAUNE){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 255;
            }
            if(imgTab[x][y]==BLEU){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 0;
            }
            if(imgTab[x][y]==ROUGE){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 255;
            }
            if(imgTab[x][y]==VERT){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 0;
            }
            if(imgTab[x][y]==BLANC){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 255;
            }
            if(imgTab[x][y]==NOIR){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 0;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 0;
            }
            if(imgTab[x][y]==UNDEFINE){
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 )    ) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 1) = 255;
                 CV_IMAGE_ELEM( imgPrint, uchar, x, ( y * 3 ) + 2) = 0;
            }

        }
    }

    cvShowImage("Mask", imgPrint);
    //cvDestroyWindow("Mask");
}


void UTCam::calibrage() {

    // Création des fenetres de seuillages
    cvNamedWindow( nameCam.c_str(), CV_WINDOW_AUTOSIZE );
    cvNamedWindow( "parametre"    , CV_WINDOW_AUTOSIZE );

    cvCreateTrackbar("num : HSV"  , "parametre", &ind      , 7, checkCouleur);
    cvCreateTrackbar("H"          , "parametre", (Htab+ind), 255, NULL);
    cvCreateTrackbar("tolerance H", "parametre", (Htol+ind), 255, NULL);
    cvCreateTrackbar("S"          , "parametre", (Stab+ind), 255, NULL);
    cvCreateTrackbar("tolerance S", "parametre", (Stol+ind), 255, NULL);
    cvCreateTrackbar("V"          , "parametre", (Vtab+ind), 255, NULL);
    cvCreateTrackbar("tolerance V", "parametre", (Vtol+ind), 255, NULL);

    cvSetMouseCallback(nameCam.c_str(), souris, this);

    // Boucle principale
    while(1) {
        frame = cvQueryFrame( ptrCamera );
        if( !frame ) break;

        maskApplication();
        afficherMask();

        //tab=Htab[ind];
        cvSetTrackbarPos("H"          , "parametre", *(Htab+ind));
        cvCreateTrackbar("tolerance H", "parametre",  (Htol+ind), 255, NULL);
        cvSetTrackbarPos("S"          , "parametre", *(Stab+ind));
        cvCreateTrackbar("tolerance S", "parametre",  (Stol+ind), 255, NULL);
        cvSetTrackbarPos("V"          , "parametre", *(Vtab+ind));
        cvCreateTrackbar("tolerance V", "parametre",  (Vtol+ind), 255, NULL);
        cvShowImage( nameCam.c_str(), frame );

        char c = cvWaitKey('q');
        if( c == 'q' ) break;
    }

    // Destruction
    cvDestroyWindow("parametre");
    cvDestroyWindow(nameCam.c_str());
}

UTCam::~UTCam() {

    cvReleaseCapture( &ptrCamera );

}

