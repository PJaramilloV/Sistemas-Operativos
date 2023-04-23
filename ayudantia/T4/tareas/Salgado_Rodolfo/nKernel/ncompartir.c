#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

static NthQueue *q;
static nThread th_comp;
static int count;

void nth_compartirInit(void) {
  q = nth_makeQueue();
  th_comp = NULL;
  count = 0;
}

void nCompartir(void *ptr) {
  START_CRITICAL
  th_comp = nSelf();
  th_comp->ptr = ptr;
  while ( !nth_emptyQueue(q) ) {
    nThread th_acc = nth_getFront(q);
    setReady(th_acc);
    schedule();
  }
  suspend(WAIT_COMPARTIR);
  schedule();
  th_comp = NULL;
  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  while ( th_comp == NULL ) {
    nth_putBack(q, nSelf());
    suspend(WAIT_ACCEDER);
    schedule();
  }
  void *ptr = th_comp->ptr;
  count++;
  END_CRITICAL
  return ptr;
}

void nDevolver(void) {
  START_CRITICAL
  count--;
  if ( (count == 0) && (th_comp != NULL) && (th_comp->status != READY) ) {
    setReady(th_comp);
    schedule();
  }
  END_CRITICAL
}
