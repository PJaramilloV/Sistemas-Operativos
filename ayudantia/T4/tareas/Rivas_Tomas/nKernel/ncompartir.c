#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

static int haycompartiendo;
static Queue *quieropediralgo;
static int pedialgoyaunnolodevuelvo;
static void *elementocompartido;
static nThread threadcompartidor;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  haycompartiendo=0;
  pedialgoyaunnolodevuelvo=0;
  quieropediralgo=makeQueue();
}

void nCompartir(void *ptr) {
  START_CRITICAL;
  threadcompartidor=nSelf();
  elementocompartido=ptr;
  haycompartiendo=1;
  if(queueLength(quieropediralgo)==0){ //Nadie quiere pedirme algo aun
    suspend(WAIT_COMPARTIR);
    schedule();
  }
  else{ //Habia gente esperando
    while(queueLength(quieropediralgo)>0){
      setReady(get(quieropediralgo));
    }
    suspend(WAIT_COMPARTIR);
    schedule();
  }
  schedule();
  END_CRITICAL;
}

void *nAcceder(int max_millis) {
  START_CRITICAL;
  pedialgoyaunnolodevuelvo++;
  if(haycompartiendo==0){
    put(quieropediralgo,nSelf());
    suspend(WAIT_ACCEDER);
    schedule();
  }
  void *retorno=elementocompartido;
  END_CRITICAL;
  return retorno;
}

void nDevolver(void) {
  START_CRITICAL;
  pedialgoyaunnolodevuelvo--;
  if (pedialgoyaunnolodevuelvo==0){
    haycompartiendo=0;
    setReady(threadcompartidor);
  }
  END_CRITICAL;
}
