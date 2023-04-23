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

// Caller for opposite sex to enter bathroom
void callIn(int sexo){
  // Call all others inside
  while (!nth_emptyQueue(nthq[sexo])){
    nThread person = nth_getFront(nthq[sexo]);
    // If they have a timer, disarm timer
    if(person->status == WAIT_PUB_TIMEOUT){
        nth_cancelThread(person);
        person->send.rc = 0;
    }
    setReady(person); // Ready to go in
    peeing[sexo]++;
    schedule();
  } 
}

// Function to remove them from the nth_queue
void wakeUpPubFunc(nThread th){
  if(nth_queryThread(nthq[0], th)){
    nth_delQueue(nthq[0], th);
  } else {
    nth_delQueue(nthq[1], th);
  }
}

// Routine for waiting outside the bathroom
void waitOutside(int sexo){
  nThread thisTh = nSelf();
  nth_putBack(nthq[sexo], thisTh); // Queue ouside
  suspend(WAIT_PUB); // wait
  schedule(); // Schedule next process
}

void nEntrar(int sexo) {
  int others = !sexo;
  START_CRITICAL // LOCK
  // If other sex is peeing or waiting in queue, i queue
  if(peeing[others] || !nth_emptyQueue(nthq[others])){  
    waitOutside(sexo);
  } else {
    peeing[sexo]++;
  }
  END_CRITICAL // UNLOCK
}

int nEntrarTimeout(int sexo, long long delayNanos) {
  // If they have an invalid timeout, they won't enter
  if(delayNanos <= 0){
    return 1;
  }
  int others = !sexo;
  START_CRITICAL
  nThread this_th = nSelf();
  this_th->send.rc = 1;

  //If we must wait
  if(peeing[others] || !nth_emptyQueue(nthq[others])){
    // Programn a timer
    nth_programTimer(delayNanos, wakeUpPubFunc);
    nth_putBack(nthq[sexo], this_th); // Queue ouside
    suspend(WAIT_PUB_TIMEOUT); // wait
    schedule(); // Schedule next process

  }else{
    peeing[sexo]++;
  }
  // Did we make it?
  int rc = this_th->send.rc;
  END_CRITICAL
  return rc;
}


void nSalir(int sexo) {
  int others = !sexo;
  START_CRITICAL
  peeing[sexo]--;
  // If all of my kind have left the bathroom
  if(peeing[sexo] == 0){
    // Tell everyone of the other kind waiting to go in
    callIn(others);
  }
  END_CRITICAL
}
