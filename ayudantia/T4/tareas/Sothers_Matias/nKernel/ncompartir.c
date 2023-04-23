#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

// ... defina aca sus variables globales con el atributo static ...
static NthQueue *accederQueue;
static nThread compartidor;
static int usando;


// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  accederQueue = nth_makeQueue();
  usando = 0;
}

void nCompartir(void *ptr) {
  START_CRITICAL

  nThread this_th = nSelf();
  compartidor = this_th;
  this_th->ptr = ptr;

  while (!nth_emptyQueue(accederQueue)){
    nThread th = nth_getFront(accederQueue);
    if(th->status == WAIT_COMPARTIR){
      setReady(th);
    }
  }
  

  suspend(WAIT_ACCEDER);
  schedule();


  compartidor = NULL;
  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL

  usando++;

  if(!compartidor){
    suspend(WAIT_COMPARTIR);
    nThread this_th = nSelf();
    nth_putBack(accederQueue, this_th);
    schedule();
  }


  void *msg = compartidor->ptr;

  

  END_CRITICAL
  return msg;
}

void nDevolver(void) {
  START_CRITICAL

  usando--;

  if(!usando && compartidor && compartidor->status == WAIT_ACCEDER){
    setReady(compartidor);
  }
  
  schedule();  


  END_CRITICAL
}
