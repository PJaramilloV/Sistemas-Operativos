#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.



static nThread t;  // thread que comparte
static NthQueue *q; // cola de threads que quieren acceder
static int n_readers; // lectores activos

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

// inicializamos la cola de acceso
void nth_compartirInit(void) {
  q = nth_makeQueue();
}

void nCompartir(void *ptr) {
  
  // vamos a trabajar con variables globales
  START_CRITICAL;
  
  // thread que comparte con el puntero asignado
  nThread this_th = nSelf();
  this_th->ptr = ptr;
  t = this_th;
  
  // revisamos si hay threads esperando acceder
  while(!nth_emptyQueue(q)) {
    // hay un nuevo lector activo y lo despertamos
    n_readers++;
    nThread th = nth_getFront(q);
    th->ptr = ptr;
    setReady(th);
  }
  
  // esperamos a que el ultimo lector llame a devolver
  suspend(WAIT_COMPARTIR);
  schedule();
  
  END_CRITICAL;
  
}

void *nAcceder(int max_millis) {
  
  // variable local para el puntero
  void *ptr;
  
  // vamos a trabajar con variables globales
  START_CRITICAL;
  
  // thread que quiere acceder
  nThread this_th = nSelf();
  
  // si no hay thread compartiendo, a mimir hasta que compartir despierte
  if(!t) {
    nth_putBack(q, this_th);
    suspend(WAIT_ACCEDER);
    schedule();
  }
  
  // aqui contamos a los threads que no salieron de la cola
  if(this_th->ptr != t->ptr) {
    this_th->ptr = t->ptr;
    n_readers++;
  }
  
  // recuperamos el puntero que viene en el thread de acceso
  ptr = this_th->ptr;
  
  END_CRITICAL;
  
  return ptr;
  
}

void nDevolver(void) {

  START_CRITICAL;
  
  // limpiamos el puntero del thread y descontamos lector activo
  nThread this_th = nSelf();
  this_th->ptr = NULL;
  n_readers--;
  
  // si no hay lectores activos
  if(!n_readers) {
    // limpiamos el thread que comparte y lo despertamos
    t->ptr=NULL;
    setReady(t);
    t=NULL;
  }
  
  END_CRITICAL;
  
}
