#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

static void *dato;
static nThread idCompartir;
static int i;
NthQueue *q;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  q = nth_makeQueue();
}

void nCompartir(void *ptr) {
  START_CRITICAL
  nThread thisThread = nSelf();
  idCompartir = thisThread;
  dato = ptr;
  if (nth_emptyQueue(q)) {
    suspend(WAIT_ACCEDER);
    schedule();
  }
  else {
     while (!nth_emptyQueue(q)) {
      nThread next = nth_getFront(q);
      if (next->status == WAIT_COMPARTIR) {
        setReady(next);
      }
    }
    suspend(WAIT_ACCEDER);
    schedule();
  }

  idCompartir = NULL;
  END_CRITICAL
  return;
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  i++;
  if (idCompartir == NULL) {
    nThread thisThread = nSelf();
    nth_putBack(q, thisThread);
    suspend(WAIT_COMPARTIR);
    schedule();
  }
  void *copia_dato = dato;
  END_CRITICAL
  return copia_dato;
}

void nDevolver(void) {
  START_CRITICAL
  i--;
  if (i == 0 && idCompartir != NULL &&  idCompartir->status == WAIT_ACCEDER) {
    setReady(idCompartir);
    schedule();
  }
  END_CRITICAL
}
