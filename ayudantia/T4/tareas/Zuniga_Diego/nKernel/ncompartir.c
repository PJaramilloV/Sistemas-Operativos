#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...
static nThread compartidor;
static NthQueue *acceder;
static int ocupando; // cantidad de nThreads que estan ocupando un compartidor


// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  compartidor = NULL;
  acceder = nth_makeQueue();
  ocupando = 0;
}

void nCompartir(void *ptr) {
  START_CRITICAL
  nThread thisTh = nSelf();
  thisTh->ptr = ptr;
  compartidor = thisTh;
  while (!nth_emptyQueue(acceder)){
    nThread th = nth_getFront(acceder);
    ocupando++;
    setReady(th);
 
  }
  suspend(WAIT_COMPARTIR);
  schedule();
  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  if (compartidor == NULL){
    nth_putBack(acceder, nSelf());
    suspend(WAIT_ACCEDER);
    schedule();
  }
  else{
    ocupando++;
  }
  END_CRITICAL
  return compartidor->ptr;
  
}

void nDevolver(void) {
  START_CRITICAL
  ocupando--;
  if (ocupando < 1){
    setReady(compartidor);
    compartidor = NULL;
    schedule();
  }
  END_CRITICAL
}
