#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.
int n_threads;
NthQueue *wait_q;
nThread remitente;
// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  n_threads=0;
  remitente = NULL;
  wait_q = nth_makeQueue();
}

void nCompartir(void *ptr) {
  START_CRITICAL;
  //llego el thread que comparte
  nThread this_th = nSelf();
  //guarda la informacion en su puntero
  this_th->ptr = ptr;
  //dejamos constancia de quien es el remitente para apagarlo despues
  remitente = this_th;
  //ahora mientras la cola de espera no este vacia
  while(!nth_emptyQueue(wait_q)){
    //vemos el thread
    nThread th = nth_getFront(wait_q);
    //los dejamos listo
    th->ptr = ptr;
    n_threads++;
    setReady(th);
  }
  //luego la cola de espera esta vacia y esperamos hasta que se mueran todos los threads
  suspend(WAIT_COMPARTIR);
  schedule();
  
  
  END_CRITICAL;
}

void *nAcceder(int max_millis) {
  //puntero a retornar
  void *retptr;
  START_CRITICAL;
  //vemos el thread actual
  nThread this_th = nSelf();
  //si nadie ha enviado un mensaje esperamos
  if (remitente==NULL){
    //lo encolamos
    nth_putBack(wait_q, this_th);
    //suspendemos esperando que llegue alguien que a hacer un compartir
    suspend(WAIT_ACCEDER);
    //hacemos scheduling para pasarle el core a otro thrread
    schedule();
  }
  //si salen del schedule es porque ya tienen el core y por lo tanto el unico que pudo dejarlo ready
  // fue el nCompartir
  //chequeamos la marca.
  if(this_th->ptr != remitente->ptr){
    this_th->ptr = remitente->ptr;
    n_threads++;
  }
  //retornamos
  retptr = this_th->ptr;
  END_CRITICAL;
  return retptr;
}

void nDevolver(void) {
  START_CRITICAL;
  //vemos el thread actual
  nThread this_th = nSelf();
  //la idea ahora es que quitamos un 1 a los mensajeados y si ahora es 0 es porque ya terminamos y le decimos al dueño
  n_threads--;
  this_th->ptr = NULL;
  //si es 0 entonces
  if(n_threads==0){
    //le decimos al remitente que borre lo que comparte
    remitente->ptr = NULL;
    //dejamos ready al remitente
    setReady(remitente);
    //y hacemos scheduling para pasarle el core al thread que comparte, pues no esta esperando nadie
    remitente=NULL;
    schedule();
  }
  
  END_CRITICAL;
}
