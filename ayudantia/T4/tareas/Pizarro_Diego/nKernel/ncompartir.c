#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// diego pizarro w


// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//static NthQueue *compartirQueue;

// Cola para almacenar a los threads que esperan para acceder al recurso

static NthQueue *accederQueue;

// usaremos esta variable para saber quien es el thread que esta compartiendo

static nThread compartidor;

// contador de cuantos estan usando el recurso

static int usando = 0;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  usando = 0;
  //compartirQueue = nth_makeQueue();
  accederQueue = nth_makeQueue();
  compartidor = NULL;
}

void nCompartir(void *ptr) {
  START_CRITICAL;

  // asignamos al que llamo a ncompartir y se pone a compartir el recurso 

  nThread thisTh = nSelf();
  compartidor = thisTh;
  thisTh->ptr = ptr;
  //nth_putBack(compartirQueue, thisTh);

  // si es que hay threads esperando a acceder los despertamos

  if(!nth_emptyQueue(accederQueue)) {
    while(!nth_emptyQueue(accederQueue)) {
      nThread th = nth_getFront(accederQueue);
      setReady(th);
    }
  }

  // el que llama a ncompartir se pone a esperar hasta que se llama a ndevolver


  suspend(WAIT_COMPARTIR);
  schedule();

  END_CRITICAL;
}

void *nAcceder(int max_millis) {
  START_CRITICAL;

  nThread thisTh = nSelf();
  usando++;

  // si es que no hay ningun thread compartiendo un recurso encolamos al 
  // thread que esta esperando a acceder

  if(compartidor == NULL) {
    nth_putBack(accederQueue, thisTh);
    suspend(WAIT_ACCEDER);
    schedule();
  }

  // si hay un thread que llamo a ncompartir adquirimos su puntero y lo retornamos

  nThread th = compartidor;
  void *ptr = th->ptr;

  END_CRITICAL;

  return ptr;
}

void nDevolver(void) {
  START_CRITICAL;

  // decrementamos el contador de cuantos estan usando el recurso

  usando--;

  // el ultimo que llamo a ndevolver notifica que el recurso ya no se usara
  // y despierta al thread que llamo a ncompartir que estba en espera

  if(usando == 0) {
    setReady(compartidor);
    compartidor = NULL;
    schedule();
  }

  END_CRITICAL;
}
