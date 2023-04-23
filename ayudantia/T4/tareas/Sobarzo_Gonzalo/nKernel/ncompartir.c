#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...
static NthQueue *q;
static nThread *compartir_thread = NULL;
static int leyendo = 0; 
// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales


void nth_compartirInit(void) {
  q = nth_makeQueue(); //queue creada  
}

void nCompartir(void *ptr) { //despertar a lectores esperando con la estrategia del while o for
  START_CRITICAL;
  if(leyendo !=0){
    while(!nth_emptyQueue(q)){ //si hay gente esperando la saco y la pongo ready para leer
      nThread th = nth_getFront(q);
      setReady(th);
    }
  }
  nThread this_th = nSelf(); //llamo al thread
  this_th->ptr = ptr;//asigno el puntero  
  compartir_thread = &this_th; //lo guardo 
  suspend(WAIT_COMPARTIR); // a dormir
  schedule(); 
  END_CRITICAL;
}

void *nAcceder(int max_millis) {
  void *ptr = NULL;
  START_CRITICAL;
  leyendo ++;
  nThread this_thread = nSelf();
  if(compartir_thread == NULL){ //si no hay nadie q comparta a esperar
    nth_putBack(q, this_thread);
    suspend(WAIT_ACCEDER);
    schedule();
  }
  nThread th = *compartir_thread;
  ptr = th->ptr;
  END_CRITICAL;
  return ptr;
}

void nDevolver(void) {
  START_CRITICAL;
  leyendo--;
  if(leyendo == 0){
    nThread th = *compartir_thread;
    compartir_thread = NULL;
    setReady(th);
    schedule();
  }
  //sino sigue quedando gente y no se pone ready al ultimo que deberia ser el que comparte
  END_CRITICAL;
}
