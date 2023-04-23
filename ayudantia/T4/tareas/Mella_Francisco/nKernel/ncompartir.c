#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

// ... defina aca sus variables globales con el atributo static ...
static NthQueue *queue;
static nThread comp;
static int nro_acc;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  queue = nth_makeQueue();
  comp = NULL;
  nro_acc = 0;
}

void nCompartir(void *ptr) {
  START_CRITICAL

  nThread thisTh = nSelf();
  thisTh->ptr = ptr;
  comp = thisTh;

  while (!nth_emptyQueue(queue)) {
    nThread nextTh = nth_getFront(queue);
    setReady(nextTh);
  }
  suspend(WAIT_COMPARTIR);
  schedule();

  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  
  nro_acc++;
  nThread thisTh = nSelf();

  if (comp==NULL) {
    nth_putBack(queue, thisTh);
    suspend(WAIT_ACCEDER);
    schedule();
  }

  void *rPtr = comp->ptr;

  END_CRITICAL
  return rPtr;
}

void nDevolver(void) {
  START_CRITICAL

  nro_acc--;
  if (nro_acc==0) {
    setReady(comp);
    comp = NULL;
  }
  
  schedule();

  END_CRITICAL
}
