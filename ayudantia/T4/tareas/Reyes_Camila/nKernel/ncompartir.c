#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...
static nThread comp_th;
static NthQueue * q_acceso;
static int cnt;


// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  /*
  necesitamos una variable global de un puntero al thread que esta compartiendo
  necesitamos una cola para poner los threads que quieran acceder 
  necesitamos un contador de threads que tienen acceso a ptr (accedieron y no han devuelto)
  */
  comp_th = NULL;
  q_acceso = nth_makeQueue();
  cnt = 0;
}

void nCompartir(void *ptr) {
  /*
  bloquear seccion critica
  compartir el ptr
  preguntar si es que hay algun thread esperando para acceder h
  

  Si es que si hay threads esperando para acceder:
      -despertarlos, ponerlos en READY (esto los saca de la cola)

  me pongo en estado wait hasta que alguien acceda y todos devuelvan

  desbloquear seccion critica 
  retornar  */
  START_CRITICAL

  nThread this_th = nSelf();
  this_th -> ptr = ptr;
  comp_th = this_th;

  while(!nth_emptyQueue(q_acceso)){
    nThread th = nth_getFront(q_acceso);
    setReady(th);
  }

  suspend(WAIT_COMPARTIR);
  schedule();
  END_CRITICAL
  return;
}

void *nAcceder(int max_millis) {
  /*
  bloquear seccion critica 
  aumentar el contador 
  preguntar si hay algun thread compartiendo

  Si es que hay algun thread compartiendo:
    -copiar ptr a una variable local

  Si no hay algun thread compartiendo:
    -me pongo en la cola 
    -me pono en estado wait
    -copiar ptr a una variable local 

  desbloquear la seccion critica 
  retornar la copia del ptr    
  */

  START_CRITICAL
  cnt++;
  void * ptr_copy;
  if(comp_th != NULL){
    ptr_copy = comp_th -> ptr;
  }

  else {
    nThread this_th = nSelf();
    nth_putBack(q_acceso, this_th);
    suspend(WAIT_ACCEDER);
    schedule();
    ptr_copy = comp_th -> ptr;
  }

  END_CRITICAL
  return ptr_copy;
}

void nDevolver(void) {
  /*
  bloquear seccion critica 
  reducir contador 

  Si el contador es 0 despierto al thread que comparte
  reiniciar variables globales

  desbloquear seccion critica
  retornar
  */
  START_CRITICAL
  cnt--;

  if(cnt == 0) {
    setReady(comp_th);
    comp_th = NULL;
    schedule();
  }

  END_CRITICAL
  return;
}
