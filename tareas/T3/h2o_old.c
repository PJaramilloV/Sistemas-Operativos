#include <pthread.h>

#include "pss.h"
#include "h2o.h"

//.. define aca las estructuras de datos que necesite ...
typedef struct {
  int ready;
  Hydrogen *h1, *h2;
  Oxygen* o;
  pthread_cond_t w;
} Request;


Queue *reqq;
Queue *oxyq;

//.. define aca las variables globales que necesite ...
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // standard mutex
Request *gloR; // Global pointer to Request to hold 1 Hydrogen requests before Queue insertion

void initH2O(void) {
  reqq = makeQueue();
  oxyq = makeQueue();
}

void endH2O(void) {
  destroyQueue(reqq);
  destroyQueue(oxyq);
}

H2O *combineOxy(Oxygen *o) {
  pthread_mutex_lock(&m);
  /* Oxygen appears, check if there's a molecule being formed
   *   --> there must be a Request missing Oxygen */
  if(emptyQueue(reqq) || (((Request*) peek(reqq))->o) != NULL){
    // There are no molecules in formation, thus it must wait
    pthread_cond_t oxc = PTHREAD_COND_INITIALIZER;
    put(oxyq, &oxc);
    // wait until there's a molecule in need of Oxygen
    while (emptyQueue(reqq) || (((Request*) peek(reqq))->o) != NULL){
      pthread_cond_wait(&oxc, &m);
    }
    get(oxyq); // unqueue the Oxygen condition
  } 
  /* There is a molecule in the making and needs Oxygen, combine to it.
   * Seek the molecule and combine the Oxygen, unqueue the Oxygen */
  Request *req = peek(reqq);
  req->o = o;
  // If there were 2 Hydrogens already, it's ready, unqueue the molecule
  if(req->h1 && req->h2){
    req->ready = 1;
    get(reqq);
    pthread_cond_signal(&(req->w));
  } else {
    // Once the remaining Hydrogen shows up, proceed, wait until then
    while(req->ready != 1)
      pthread_cond_wait(&(req->w), &m);
  }
  Hydrogen *h1 = req->h1;
  Hydrogen *h2 = req->h2;
  
  pthread_mutex_unlock(&m);
  return makeH2O(h1, h2, o);
}

H2O *combineHydro(Hydrogen *h) {
  pthread_mutex_lock(&m);
  Hydrogen *h1;
  Hydrogen *h2;
  Oxygen *o;
  // New Hydrogen, is there a molecule in the making?
  if(emptyQueue(reqq) || (((Request*) peek(reqq))->h2) != NULL){
    // no molecules or waiting molecule has H_2

    // Check if the auxiliar pointer is available
    if(gloR == NULL){
      // Create new Request, assign to global
      Request localR = {0, h, NULL, NULL, PTHREAD_COND_INITIALIZER};
      gloR = &localR; // Assign to global so other threads can access
      // wait using localR, we expect the 2nd Hydrogen thread to 
      // queue this request and empty the global pointer
      while (!(localR.ready)){
        pthread_cond_wait(&(localR.w), &m);
      }
      h1 = localR.h1;
      h2 = localR.h2;
      o = localR.o;
    } else {
      // There is a molecule waiting outside of Queue
      // give it the last hydrogen
      gloR->h2 = h;
      Request *localR = gloR;
      // If the last hydrogen was all that was needed, wake the molecule!
      if(gloR->o != NULL){
        gloR->ready = 1;
        pthread_cond_signal(&(gloR->w));
        gloR = NULL; // Clear the global variable
      } else {
        // Molecule has 2 Hydrogen but needs Oxygen, queue it!
        put(reqq, &localR);
        // Wake first come Oxygen if it's there
        if(!emptyQueue(oxyq)){
          pthread_cond_t *oxc = peek(oxyq);
          pthread_cond_signal(oxc);
        }
        // Clear the global variable, the molecule is queued, 
        // leave this pointer for new molecules
        gloR = NULL; 
        // Wait until we receieve Oxygen
        while(!(localR->ready))
          pthread_cond_wait(&(localR->w), &m);
        get(reqq); // Unqueue the request
      }
    h1 = localR->h1;
    h2 = localR->h2;
    o = localR->o;
    }
  } else {
    // there is a molecule waiting at the queue and needs Hydrogen
    Request *localR = peek(reqq);
    localR->h2 = h;
    // The Oxygen was already present
    if(localR->o != NULL){
      localR->ready = 1;
      pthread_cond_signal(&(localR->w));
      get(reqq); // Unqueue request
    } else {
      while (!(localR->ready)){
        pthread_cond_wait(&(localR->w),&m);
      }
    }
    h1 = localR->h1;
    h2 = localR->h2;
    o = localR->o;
  }

  pthread_mutex_unlock(&m);
  return makeH2O(h1, h2, o);
}
