#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

int reading;
int writing;
void *data;
nThread writer;
NthQueue *readers;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
    reading = 0;
    writing = 0;
    readers = nth_makeQueue();
}

void nCompartir(void *ptr) {
    START_CRITICAL
    data = ptr;
    writing=1;
    writer = nSelf();
    if(reading>0){
        nThread next;
        while(!nth_emptyQueue(readers)){
            next = nth_getFront(readers);
            setReady(next);
        }
    }
    suspend(WAIT_COMPARTIR);
    schedule();
    END_CRITICAL
}

void *nAcceder(int max_millis) {
    START_CRITICAL
    reading++;
    void *answer;
    if(writing==0){
        nThread th= nSelf();
        nth_putBack(readers, th);
        suspend(WAIT_ACCEDER);
        schedule();
    }
    answer = data;
    END_CRITICAL
    return answer;
}

void nDevolver(void) {
    START_CRITICAL
    reading--;
    if(reading==0){
        writing=0;
        setReady(writer);
    }
    END_CRITICAL
}
