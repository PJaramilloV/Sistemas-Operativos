// bali pero que siempre funcione bien
// 2 colas fifo
// si un hincha espera crea un semaforo con 0 tickets
// y pone este en la cola correspondoente, suspender thread

//si sale el ultimo hincha del equipo y hay del otro en espera, se ssacan 
// todos los semaforos de esa cola


#include "pss.h"        // Queue implementado aqui
#include <semaphore.h>

enum {ROJO = 0, AZUL = 1};
int mutex = 0;
int adentro[2] = {0,0};

Queue* queues = {makeQueue(), makeQueue()};

sem_t mutex;
sem_init(&mutex, 0, 1);
sem_t sem[2]:
sem_init(&sem[ROJO], 0, 1);
sem_init(&sem[AZUL], 0, 1);

void entrar(int color){
    int oponente = (color +1) % 2;

    sem_wait(&mutex); // lock
    if(adentro[oponente] >0  ||  !emptyQueue(queues[oponente]) ){ // si no hay oponentes en el baño o esperando entrar, entro

        sem_t wait;
        sem_init(&wait, 0, 0); // 0 tickets
        put(queues[color], &wait); // ponemos el semaforo en cola

        sem_post(&mutex);
        sem_wait(&wait); // suspender thread

        sem_destroy(&wait);
    }else{
        adentro[color]++; // entrar al baño
        sem_post(&mutex); // unlock
    }


}

void salir(int color){
    int oponente = (color +1) % 2;

    sem_wait(&mutex); // lock

    adentro[color]--; // salir del baño

    if(adentro[color] == 0) {// Si mi equipo deja el baño
        while (!emptyQueue(queues[oponente])){
            sem_t *wait = get(queues[oponente]);
            sem_post(&wait);
            adentro[oponente]++;
        }
    }

    sem_post(&mutex); // unlock
}