#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...

static NthQueue *q; //cola
static int esperando = 0; //cantidad de threads en espera
static nThread *threadGlobal = NULL; //el thread que comparte


// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  q = nth_makeQueue();
}

void nCompartir(void *ptr) {
  START_CRITICAL
  
  nThread this = nSelf();
  threadGlobal = &this; //dirección del thread 
  if(!nth_emptyQueue(q)) {
    while(!nth_emptyQueue(q)) {
      nThread th = nth_getFront(q);
      setReady(th);
      schedule();   
    }
  }
  
  this->ptr = ptr; 
  suspend(WAIT_ACCEDER);
  schedule();
  threadGlobal = NULL; //puntero a NULL, al terminar

  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  
  nThread this = nSelf();
  esperando = esperando + 1;
  void *local_ptr;

  if (threadGlobal == NULL) {
    nth_putBack(q, this);
    suspend(WAIT_COMPARTIR);
    schedule();
    local_ptr = this->ptr;
  }
  
  nThread sender_th = *threadGlobal;
  local_ptr = sender_th->ptr;
  return local_ptr;
  
  END_CRITICAL

}

void nDevolver(void) {
  START_CRITICAL

  esperando = esperando - 1;
  nThread th = *threadGlobal;

  if (esperando == 0 && th->status == WAIT_ACCEDER) {
    setReady(*threadGlobal);
    schedule();
  }

  END_CRITICAL
}
