// baño pero sin hambruna

#include <semaphore.h>

enum {ROJO = 0, AZUL = 1};
int mutex = 0;
int cantidad[2] = {0,0};
int esperan[2] = {0,0};
int adentro[2] = {0,0};

sem_t mutex;
sem_init(&mutex, 0, 1);
sem_t sem[2]:

void entrar(int color){
    int oponente = (color +1) % 2;

    sem_wait(&mutex); // lock
    if(adentro[oponente] >0  ||  esperan[oponente] > 0){ // si no hay oponentes en el baño o esperando entrar, entro
        sem_post(&mutex);
        sem_wait(&sem[color]);
        sem_wait(&mutex);
    }
    adentro[color]++; // entrar al baño
    sem_post(&mutex); // unlock
}

void salir(int color){
    int oponente = (color +1) % 2;

    sem_wait(&mutex); // lock

    adentro[color]--; // salir del baño

    if(adentro[color] == 0) {// Si mi equipo deja el baño
        for(int i = 0; i<esperan[oponente]; i++) {
            sem_post(sem[oponente]); // notificar a todo el equipo oponente
        }
        esperan[oponente] = 0;
    }
    sem_post(&mutex); // unlock
}
