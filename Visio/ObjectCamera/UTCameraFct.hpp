/****************************************************
*****               DEFINE CODE                 *****
****************************************************/
// TrackBar
#define TRACKBARS               cvCreateTrackbar("NOIR  : tolerance H" , "parametre 1", Htol+NOIR, 255, NULL);\
                                cvCreateTrackbar("NOIR  : tolerance S" , "parametre 1", Stol+NOIR, 255, NULL);\
                                cvCreateTrackbar("NOIR  : tolerance V" , "parametre 1", Vtol+NOIR, 255, NULL);\
                                cvCreateTrackbar("ROUGE : tolerance H" , "parametre 1", Htol+ROUGE, 255, NULL);\
                                cvCreateTrackbar("ROUGE : tolerance S" , "parametre 1", Stol+ROUGE, 255, NULL);\
                                cvCreateTrackbar("ROUGE : tolerance V" , "parametre 1", Vtol+ROUGE, 255, NULL);\
                                cvCreateTrackbar("VERT  : tolerance H" , "parametre 2", Htol+VERT,  255, NULL);\
                                cvCreateTrackbar("VERT  : tolerance S" , "parametre 2", Stol+VERT,  255, NULL);\
                                cvCreateTrackbar("VERT  : tolerance V" , "parametre 2", Vtol+VERT,  255, NULL);\
                                cvCreateTrackbar("BLEU  : tolerance H" , "parametre 2", Htol+BLEU,  255, NULL);\
                                cvCreateTrackbar("BLEU  : tolerance S" , "parametre 2", Stol+BLEU,  255, NULL);\
                                cvCreateTrackbar("BLEU  : tolerance V" , "parametre 2", Vtol+BLEU,  255, NULL);\
                                cvCreateTrackbar("JAUNE : tolerance H" , "parametre 1", Htol+JAUNE, 255, NULL);\
                                cvCreateTrackbar("JAUNE : tolerance S" , "parametre 1", Stol+JAUNE, 255, NULL);\
                                cvCreateTrackbar("JAUNE : tolerance V" , "parametre 1", Vtol+JAUNE, 255, NULL);\
                                cvCreateTrackbar("BLANC : tolerance H" , "parametre 2", Htol+BLANC, 255, NULL);\
                                cvCreateTrackbar("BLANC : tolerance S" , "parametre 2", Stol+BLANC, 255, NULL);\
                                cvCreateTrackbar("BLANC : tolerance V" , "parametre 2", Vtol+BLANC, 255, NULL);

// Calcul des seuils avec la tolerance
#define SEUIL(nb, cherche, tol) (nb > (cherche-tol) && nb < (cherche+tol))
// Renvoie true si la couleur en hsv est dans les seuils de tolerance de color.
#define IS_COLOR(h,s,v,color)   (SEUIL(h, data->Htab[color], data->Htol[color]) && \
                                 SEUIL(s, data->Stab[color], data->Stol[color]) && \
                                 SEUIL(v, data->Vtab[color], data->Vtol[color]) )
// Charge la valeur d'un pixel 1 channel
#define SET1_PIXEL(f,x,y,v)      CV_IMAGE_ELEM( f, uchar, y, x ) = v;
// Charge la valeur d'un pixel
#define SET3_PIXEL(f,x,y,r,g,b)  CV_IMAGE_ELEM( f, uchar, y, ( x * 3 )   ) = b; \
                                 CV_IMAGE_ELEM( f, uchar, y, ( x * 3 ) +1) = g; \
                                 CV_IMAGE_ELEM( f, uchar, y, ( x * 3 ) +2) = r;


/****************************************************
*****                FONCTIONS                  *****
****************************************************/

// **************************************************
// * Fonction qui permet d'éffectuer la binarisation*
void* binThread(void* pData)
{
    // Récupération des données
    int h,s,v,ymax;
    threaddata* data = (threaddata*)pData;
    ymax = data->firstRow + data->nbRow;

    // Traitement
    for(int x=0 ; x<data->width ; x++){
        for(int y=data->firstRow ; y<ymax ; y++){

            // Valeur du pixel
            h = CV_IMAGE_ELEM( data->src, uchar, y, ( x * 3 )   );
            s = CV_IMAGE_ELEM( data->src, uchar, y, ( x * 3 ) +1);
            v = CV_IMAGE_ELEM( data->src, uchar, y, ( x * 3 ) +2);

            // Traitement
/*            if(IS_COLOR(h,s,v,BLEU))
                { SET1_PIXEL(data->dst,x,y,255); }
            else if(IS_COLOR(h,s,v,ROUGE))
                { SET1_PIXEL(data->dst,x,y,255); }
            else if(IS_COLOR(h,s,v,VERT))
                { SET1_PIXEL(data->dst,x,y,255); }
            else*/
            if(IS_COLOR(h,s,v,JAUNE))
                { SET1_PIXEL(data->dst,x,y,0  );}
/*            else if(IS_COLOR(h,s,v,NOIR))
                { SET1_PIXEL(data->dst,x,y,255); }
            else if(IS_COLOR(h,s,v,BLANC))
                { SET1_PIXEL(data->dst,x,y,255); } */
            else
                { SET1_PIXEL(data->dst,x,y,255);}
        }
    }
}




// **************************************************
// * Fonction qui permet d'éffectuer la colorisation*
void* colThread(void* pData)
{
    // Récupération des données
    int h,s,v,ymax;
    threaddata* data = (threaddata*)pData;
    ymax = data->firstRow + data->nbRow;

    // Traitement
    for(int x=0 ; x<data->width ; x++){
        for(int y=data->firstRow ; y<ymax ; y++){

            // Valeur du pixel
            h = CV_IMAGE_ELEM( data->src, uchar, y, ( x * 3 )   );
            s = CV_IMAGE_ELEM( data->src, uchar, y, ( x * 3 ) +1);
            v = CV_IMAGE_ELEM( data->src, uchar, y, ( x * 3 ) +2);

            //
            if(IS_COLOR(h,s,v,BLEU))
                { SET3_PIXEL(data->dst,x,y,0  ,0  ,255); }
            else if(IS_COLOR(h,s,v,ROUGE))
                { SET3_PIXEL(data->dst,x,y,255,0  ,  0); }
            else if(IS_COLOR(h,s,v,VERT))
                { SET3_PIXEL(data->dst,x,y,0  ,255,  0); }
            else if(IS_COLOR(h,s,v,JAUNE))
                { SET3_PIXEL(data->dst,x,y,255,255,  0); }
            else if(IS_COLOR(h,s,v,NOIR))
                { SET3_PIXEL(data->dst,x,y,0  ,  0,  0); }
            else if(IS_COLOR(h,s,v,BLANC))
                { SET3_PIXEL(data->dst,x,y,255,255,255); }
            else
                { SET3_PIXEL(data->dst,x,y,0  ,255,255);}
        }
    }

}

// **************************************************
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

        //cout << "H-S-V" << "    " << scal.val[0] << "-" << scal.val[1] << "-" << scal.val[2] << endl;
    }

}

// **************************************************
// ********  Fonction pour étendre une zorame=cvQueryFramne  ********
void extendRegion(region* z, int width, int height, IplImage* binFrame, int x, int y, int stop)
{

    if(stop<STOP_REC){
		(z->nbPixel)++;
        CV_IMAGE_ELEM( binFrame, uchar, y, x ) = 255;

		if( y < z->max[HAUT     ].y ) { z->max[HAUT     ] = cvPoint(x,y); }
		if( y > z->max[BAS      ].y ) { z->max[BAS      ] = cvPoint(x,y); }
		if( x > z->max[DROITE   ].x ) { z->max[DROITE   ] = cvPoint(x,y); }
		if( x < z->max[GAUCHE   ].x ) { z->max[GAUCHE   ] = cvPoint(x,y); }

		if( (x+1<height) ) {
            if( CV_IMAGE_ELEM( binFrame, uchar, y, (x+1) ) == 0 ){
                extendRegion(z, width, height, binFrame, x+1, y, stop+1);
			}
		}
		if( (x-1>=0) ) {
		    if( CV_IMAGE_ELEM( binFrame, uchar, y, (x-1) ) == 0 ){
                extendRegion(z, width, height, binFrame, x-1, y, stop+1);
            }
		}
		if( (y+1<width) ) {
		    if( CV_IMAGE_ELEM( binFrame, uchar, (y+1), x ) == 0 ){
                extendRegion(z, width, height, binFrame, x, y+1, stop+1);
            }
		}
		if( (y-1>=0) ) {
            if( CV_IMAGE_ELEM( binFrame, uchar, (y-1), x ) == 0 ){
                extendRegion(z, width, height, binFrame, x, y-1, stop+1);
            }
		}

    }
}




