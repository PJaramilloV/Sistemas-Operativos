#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

//... defina aca las variables globales que necesite ...
//... como por ejemplo las NthQueue para damas y varones ..


static int peeing[2]= { 0, 0 };
static NthQueue* nthq[2];

void nth_iniPub(void) {
  nthq[0] = nth_makeQueue();
  nthq[1] = nth_makeQueue();
}

void nth_endPub(void) {
  //... probablemente no necesite liberar nada ...
}

void enqueue(int sexo){
  nThread thisTh = nSelf();
  nth_putBack(nthq[sexo], thisTh);
  suspend(WAIT_WC);
  schedule();
}

void wakeUp(int sexo){
  while (!nth_emptyQueue(nthq[sexo])){
      nThread person = nth_getFront(nthq[sexo]);
      setReady(person);
      schedule();
  }
}

void nEntrar(int sexo) {
  int others = !sexo;
  START_CRITICAL // LOCK
  
  // If other sex is peeing or waiting in queue, i queue
  if(peeing[others] || !nth_emptyQueue(nthq[others])){
    enqueue(sexo);
  } else {
    peeing[sexo]++;
  }
  END_CRITICAL // UNLOCK
}

void nSalir(int sexo) {
  int others = !sexo;
  START_CRITICAL
  peeing[sexo]--;
  // If all of my kind have left the bathroom
  if(peeing[sexo] == 0){
    // Tell everyone of the other kind waiting to go in
    wakeUp(others); 
  }
  END_CRITICAL
}
