/**
 * \file constante.hpp
 * \brief définit les constantes du projets
 *
 * \author Xavier rodrigueZ
 * \date 02/04/2011
 */
 #ifndef CONSTANTES_H
 #define CONSTANTES_H

 /**
 Adresse :

 Quentin Guilleus
 Résidence marie caroline studio 113
 4 square jean mermoz, Compiègne 60000
 */

// Fichiers externes

// Dossiers contenant tous les fichiers
    #define     MAIN_DIR    "CamFiles/"

// Numero de série des cameras
    #define SERIAL_CAM_F        "0A124C80"
    #define SERIAL_CAM_B        "96124C80"

// Valeurs identifiants les positions des maximum des regions
    #define HAUT 	                    0
    #define BAS 	                    1
    #define DROITE 	                    2
    #define GAUCHE 	                    3

// Type des zones
    #define TYPE_PIONT                  'p'
    #define TYPE_TOUR                   't'

// Images
    #define     IMG_WIDTH       640
    #define     IMG_HEIGHT      480

// Gestion des fonctions
    // Threading
    #define     NB_THREAD       4

    // Reccursivité extend region
    #define STOP_REC            50000

// Analyse des pions
    #define     RAYON_PIONT     100

// Gestion des Couleurs.
    #define     NB_COLOR        1

    #define     JAUNE           0

    #define     JAUNE_          200
    #define     UNDEFINE_       255

// Macro de calcul
    #define CARRE(x)       ((x)*(x))
    #define ABS(x)         ( ((x)<0)  ?   ((-1)*(x)): (x)       )
    #define DIVSUP(n,d)    ( (n%d)==0 ?   ((n)/d) : (((n)+1)/n) )
    #define DIVINF(n,d)    ( (n%d)==0 ?   ((n)/d) : (((n)-1)/n) )

// Juste utile
    #define STD_POLICE      CvFont font;\
                            double hScale=0.5;\
                            double vScale=0.5;\
                            int    lineWidth=1;\
                            cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);

    // Calcul des seuils avec la tolerance
    #define SEUIL(nb, cherche, tol) (nb > (cherche-tol) && nb < (cherche+tol))
    // Renvoie true si la couleur en hsv est dans les seuils de tolerance de color.
    #define IS_COLOR(h,s,color)   (SEUIL(h, _H[color], _Htol[color]) && \
                                   SEUIL(s, _S[color], _Stol[color]) )
    // Charge la valeur d'un pixel 1 channel
    #define SET1_PIXEL(f,x,y,v)    CV_IMAGE_ELEM( f, uchar, y, x ) = v;

 #endif


