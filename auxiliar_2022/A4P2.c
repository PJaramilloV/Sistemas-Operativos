// hacer el problema del baño correcta y eficientemente
// no dataraces ni busy waiting
// starvation esta permitido

#include <semaphore.h>

enum {ROJO = 0, AZUL = 1};
int mutex = 0;
int cantidad[2] = {0,0};

sem_t mutex;
sem_init(&mutex, 0, 1);
sem_t sem[2]:

void entrar(int color){
    int oponente = (color +1) % 2;

    sem_wait(&mutex); // lock
    if(cantidad[oponente] > 0){ // si no hay oponentes en el baño
        sem_post(&mutex);
        sem_wait(&sem[color]);
        sem_wait(&mutex);
    }
    cantidad[color]++; // entrar al baño

    sem_post(&mutex); // unlock
}

void salir(int color){
    int oponente = (color +1) % 2;

    sem_wait(&mutex); // lock

    cantidad[color]--; // salir del baño

    if(cantidad[color] == 0) // Si mi equipo deja el baño
        for(int i = 0; i<cantidad[oponente]; i++) {
            sem_post(sem[oponente]); // notificar a todo el equipo oponente
        }

    sem_post(&mutex); // unlock
}

// idea: 1 semaforo para exclusion mutua
//       1 semaforo para hacer waiting x equipo