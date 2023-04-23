// Crear un sistema para colectar $X pesos
#include <pthread.h>


typedef struct {
    double meta;
    double fondo;
    pthread_mutex_t m;
    pthread_cond_t c;
} Colecta;


Colecta *newColecta(double meta){
    Colecta *colecta = (Colecta*) malloc(sizeof(Colecta));
    colecta-> meta = meta;
    colecta->fondo = 0; 
    pthread_mutex_init(&colecta->m, NULL);
    pthread_cond_init(&colecta->c, NULL);
    return colecta;
}

// Imaginar que estamos en un ambiente de threads, donde tenemos que programar la seccion critica
double aportar(Colecta *colect, double monto){
    pthread_mutex_lock(&colect->m); // SECCION CRITICA

    if(monto + colect->fondo < colect->meta)
        colect->fondo += monto;
    else{
        colect->fondo += colect->meta - colect->fondo ;
        pthread_cond_broadcast(&colect->c);
    }

    while (colect->meta > colect->fondo){ // Dormir thread, se libera implicitamente el mutex
        pthread_cond_wait(&colect->c, &colect->m);
    }
    pthread_mutex_unlock(&colect->m); // Liberar mutex
    return monto;
}