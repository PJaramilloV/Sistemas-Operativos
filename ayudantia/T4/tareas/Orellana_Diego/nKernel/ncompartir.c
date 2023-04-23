#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...

static NthQueue *q;  //cola para almacenar los threads en espera de nAcceder
static int esperando = 0; //cantidad de threads en espera
static nThread *threadGlobal = NULL; //variable que almacena el thread que comparte


// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  q = nth_makeQueue();
}

void nCompartir(void *ptr) {
  START_CRITICAL
  
  nThread this_th = nSelf();
  threadGlobal = &this_th; // guardamos la dirección del thread
  // Despertamos a los threads de nAcceder que estén esperando (si es que hay)
  // Y les entregamos el puntero 
  if(!nth_emptyQueue(q)) {
    while(!nth_emptyQueue(q)) {
      nThread th = nth_getFront(q);
      setReady(th);
      schedule();   
    }
  }
  
  this_th->ptr = ptr; // "guardamos" el puntero dentro del nThread
  
  // esperar respuesta hasta que no hayan threads que quieran acceder
  // Entramos a la cola, para que los threads nuevos que lleguen
  // obtengan el puntero
  
  suspend(WAIT_ACCEDER);
  schedule();
  threadGlobal = NULL; //cuando terminamos, se resetea el puntero a NULL

  END_CRITICAL
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  
  nThread this_th = nSelf();
  esperando = esperando + 1;
  void *local_ptr;

  // si no hay un nAcceder en espera, entonces entramos a la cola y esperamos
  if (threadGlobal == NULL) {
    nth_putBack(q, this_th);
    suspend(WAIT_COMPARTIR);
    schedule();
    local_ptr = this_th->ptr; //cuando despierta, obtenemos el puntero
  }
  
  // obtenemos el puntero a través de la variable global
  nThread sender_th = *threadGlobal; // obtenemos el thread 
  local_ptr = sender_th->ptr;  // obtenemos el puntero
  return local_ptr; // retornamos el puntero
  
  END_CRITICAL

}


void nDevolver(void) {
  START_CRITICAL

  esperando = esperando - 1;
  nThread th = *threadGlobal;
  //si el contador es 0, entonces enviamos la señal a nCompartir para que termine
  if (esperando == 0 && th->status == WAIT_ACCEDER) {
    setReady(*threadGlobal);
    schedule();
  }

  END_CRITICAL
}
