#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

NthQueue * waitQueue;
nThread  sharing_th = NULL;
int readers= 0;
// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  waitQueue = nth_makeQueue();
}

void nCompartir(void *ptr) {
  START_CRITICAL
  if (sharing_th!=NULL){
    return;
  }
  nThread this_th = nSelf();
  this_th->ptr = ptr;
  sharing_th = this_th;
  while(!nth_emptyQueue(waitQueue)){
    nThread next_th = nth_getFront(waitQueue);
    setReady(next_th);
  }
  suspend(WAIT_COMPARTIR);
  schedule();
  sharing_th = NULL;
  END_CRITICAL;
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  nThread this_th = nSelf();
  readers++;
  if (sharing_th==NULL){
    suspend(WAIT_ACCEDER);
    nth_putBack(waitQueue, this_th);
    schedule();
  }
  void* ptr = sharing_th->ptr;
  END_CRITICAL
  return ptr;
}

void nDevolver(void) {
  START_CRITICAL
  readers--;
  if (!readers && sharing_th!=NULL && (sharing_th->status!=READY)){
    setReady(sharing_th);
  }
  END_CRITICAL
}
