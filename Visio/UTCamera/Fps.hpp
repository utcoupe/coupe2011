#ifndef FPS_HPP
#define FPS_HPP

#ifdef __unix__
#    include <time.h>
#    include <sys/time.h>
#else
#    include <windows.h>
#endif

class Fps {

private:
    double debutMesure,finMesure,fps,milli;

public:

#ifdef __unix__
#    include <time.h>
#    include <sys/time.h>
#else
#    include <windows.h>
#endif

    long long getTimeMillis() {
#ifdef __unix__
        // --------------- Sous GNU/Linux ---------------

        // On récupère l'heure dans une structure "timeval"
        timeval tv;
        gettimeofday( &tv, NULL );

        // On la convertit en millisecondes à partir des secondes et
        // des microsecondes
        long long l = tv.tv_usec / 1000;
        l += tv.tv_sec * 1000;

#else
        // ---------------- Sous Windows ----------------

        // La fonction GetTickCount() de windows.h nous
        // donne directement un timestamp en millisecondes
        long long l= GetTickCount();
#endif

        return l;
    }


    void debutMesureTime() {
        debutMesure=getTimeMillis();
    }

    void finMesureTime() {
        finMesure=getTimeMillis();
    }

    double getfps() {
        // Frame rate : 1 / temps écoulé (en secondes)
        fps = 1000/(finMesure - debutMesure);
        // On met à jour le début de la mesure
        debutMesure = finMesure;
        return fps;
    }

    double getMilli() {
        // Frame rate : 1 / temps écoulé (en secondes)
        milli = (finMesure - debutMesure);
        // On met à jour le début de la mesure
        debutMesure = finMesure;
        return milli;
    }

};

#endif
