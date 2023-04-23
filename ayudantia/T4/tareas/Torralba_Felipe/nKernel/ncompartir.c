#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

static NthQueue *acceder_queue;
static nThread compartir_nthread;
static int acceder_counter;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  acceder_queue = nth_makeQueue();
  compartir_nthread = NULL;
  acceder_counter = 0;
}

void nCompartir(void *ptr) {
  START_CRITICAL;
  compartir_nthread = nSelf();
  compartir_nthread->ptr = ptr;
  while(!nth_emptyQueue(acceder_queue)){
    nThread acc = nth_getFront(acceder_queue);
    setReady(acc);
  }
  suspend(WAIT_COMPARTIR);
  schedule();
  END_CRITICAL
  
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  nThread thisTh = nSelf();
  while (compartir_nthread == NULL){
    suspend(WAIT_ACCEDER);
    nth_putBack(acceder_queue, thisTh);
    schedule();
  }
  acceder_counter++;
  END_CRITICAL
  return compartir_nthread->ptr;

}

void nDevolver(void) {
  START_CRITICAL
  acceder_counter--;
  if (acceder_counter==0){
    setReady(compartir_nthread);
    compartir_nthread = NULL;
  }
  END_CRITICAL
}
