#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

static NthQueue *q;
static nThread nSharer;
static int using;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  q = nth_makeQueue();
  using = 0;
  nSharer = NULL;
}

void nCompartir(void *ptr) {
  START_CRITICAL

  nThread this_th = nSelf();
  this_th->ptr = ptr;
  nSharer = this_th;

  if (!nth_emptyQueue(q)) {
    nThread th;
    while(!nth_emptyQueue(q)) {
      th = nth_getFront(q);
      if(th->status==WAIT_COMPARTIR) {
        setReady(th);
      }
    }
  }

  suspend(WAIT_ACCEDER);
  schedule();

  END_CRITICAL

  return;
}

void *nAcceder(int max_millis) {
  START_CRITICAL

  using += 1;

  if(nSharer==NULL) {
    nThread this_th = nSelf();
    nth_putBack(q, this_th);
    suspend(WAIT_COMPARTIR);
    schedule();
  }
  nThread sharer = nSharer;
  void *msg = sharer->ptr;
  schedule();

  END_CRITICAL

  return msg;
}

void nDevolver(void) {
  START_CRITICAL
      
  using -= 1;
  if(using==0) {
    setReady(nSharer);
    nSharer = NULL;
    schedule();
  }

  END_CRITICAL
}