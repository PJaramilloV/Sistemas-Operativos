#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

// Necesitamos una forma de saber:
// - si quedan nAcceder() ejecutadose
static int accederContador;

// necesitamos una forma de que los otros thread puedan acceder 
// al thread que esta compartiendo la informacion
static nThread *compartiendo;

// Una cola de nAcceder que estan esperando que aparezca compartir
NthQueue *accederEspera;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  accederContador = 0;
  accederEspera = nth_makeQueue();
  compartiendo = NULL;
}

void nCompartir(void *ptr) {
  /*
  Ofrece compartir los datos apuntados por ptr con los threads que llamen a nAcceder
  Espera hasta que los threads notifiquen que desocuparon los datos llamando a nDevolver
  */

  START_CRITICAL
  nThread this_th = nSelf();

  // Ponemos el puntero del thread que esta compartiendo el mensaje
  compartiendo = &this_th;

  // Le asignamos el valor ptr a el campo ptr del thread que ha llamado a nCompartir
  this_th->ptr = ptr;

  // Dejamos READY a todos los thread que estaban esperando que llegara un nCompartir
  while(!nth_emptyQueue(accederEspera) ){
    nThread th = nth_getFront(accederEspera);
    if(th->status == WAIT_ACCEDER){
      setReady(th);
      schedule();
    }
  }

  // Espera el reply de nDevolver
  suspend(WAIT_COMPARTIR); 
  schedule();
  
  compartiendo = NULL;
  END_CRITICAL
}

void *nAcceder(int max_millis) {  
  START_CRITICAL
  
  nThread this_th = nSelf();
  accederContador ++;

  // Si no se han compartido mensajes esperamos
  if(compartiendo == NULL){
    nth_putBack(accederEspera, this_th);
    suspend(WAIT_ACCEDER);
    schedule();
  }
  void *ptr = (*compartiendo)->ptr;

  END_CRITICAL
  return ptr;
}

void nDevolver(void) {
  START_CRITICAL
  accederContador --;
  if(accederContador == 0 && compartiendo != NULL){
    if( (*compartiendo)->status == WAIT_COMPARTIR)
      setReady(*compartiendo);
  }
  schedule();
  END_CRITICAL
}
