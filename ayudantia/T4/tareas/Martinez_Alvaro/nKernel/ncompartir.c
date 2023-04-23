#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

static nThread *nthreadG;
static int cant_acc;
static NthQueue *enFil;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
    nthreadG = NULL;
    cant_acc = 0;
    enFil = nth_makeQueue();
}

void nCompartir(void *ptr) {
    START_CRITICAL

    nThread thisTh= nSelf();
    thisTh->ptr = ptr;
    nthreadG = &thisTh;
    while(!nth_emptyQueue(enFil)){
        setReady(nth_getFront(enFil));
    }
    suspend(WAIT_COMPARTIR);
    schedule();
    END_CRITICAL
    return;
}

void *nAcceder(int max_millis) {
    START_CRITICAL
    cant_acc++;
    if (nthreadG == NULL){
        
        nThread thisTh= nSelf();
        nth_putBack(enFil,thisTh);
        suspend(WAIT_ACCEDER);
        schedule();
    }
    void* memory = (*nthreadG)->ptr;
    END_CRITICAL
    return memory;
}

void nDevolver(void) {
    START_CRITICAL
    cant_acc--;
    if (cant_acc != 0){
        END_CRITICAL
        return;
    }
    setReady(*nthreadG);
    //schedule();
    nthreadG = NULL;
    END_CRITICAL
    return;
}
