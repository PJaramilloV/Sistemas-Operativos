#include <pthread.h>

#include "pss.h"
#include "h2o.h"
#include "stdio.h"

//.. define aca las estructuras de datos que necesite ...
typedef enum {Hy, Ox} Type; 

typedef struct {
  int rdy;
  Hydrogen* h;
  Oxygen* o;
  H2O* water;
  pthread_cond_t w;
} Request;


//.. define aca las variables globales que necesite ...
Hydrogen* sdn_hydrogen;
Oxygen* oxygen;
Queue* queues[2];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void initH2O(void) {
  sdn_hydrogen = NULL;
  oxygen = NULL;
  queues[0] = makeQueue(); 
  queues[1] = makeQueue();  
}

void endH2O(void) {
  destroyQueue(queues[Hy]);
  destroyQueue(queues[Ox]);
  pthread_mutex_destroy(&m);
}

H2O* enqueue(Hydrogen *h, Oxygen *o, Type type){
  Request req = {0, h, o, NULL, PTHREAD_COND_INITIALIZER};
  put(queues[type], &req);
  while(!req.rdy)
    pthread_cond_wait(&req.w, &m);
  
  return (req.water);
}

H2O* enqueueH(Hydrogen *h){
  H2O* h2o = enqueue(h, NULL, Hy);
  return h2o;
}

H2O* enqueueO(Oxygen *o){
  H2O* h2o = enqueue(NULL, o, Ox);
  return h2o;
}

H2O* wakeup(Type type){
  Request *pr = peek(queues[type]);
  if(pr == NULL){
    return NULL;
    }
  if(type == Hy){
    // By construction Hydrogens get woken up only if their queueLength is >= 2
    get(queues[Hy]);
    Request *hpr2 = peek(queues[Hy]);
    get(queues[Hy]);
    // Get oxygen from global var
    Oxygen *ox = NULL;
    ox = oxygen;
    // Make water and distribute
    H2O *h2o = makeH2O((pr->h), (hpr2->h), ox);
    pr->water = h2o;
    hpr2->water = h2o;
    // Set requests as ready and wake up threads
    pr->rdy = 1;
    hpr2->rdy = 1;
    pthread_cond_signal(&(pr->w));
    pthread_cond_signal(&(hpr2->w));
    return h2o;
  }
  if(type == Ox){
    // An Oxygen was called to wakeup
    get(queues[Ox]);
    Request *hpr1 = peek(queues[Hy]);
    get(queues[Hy]);
    // Get hydrogen from global var
    Hydrogen* hy = NULL;
    hy = sdn_hydrogen;
    // Make water and distribute
    H2O *h2o = makeH2O((hpr1->h), hy, (pr->o));
    pr->water = h2o;
    hpr1->water = h2o;
    // Set requests as ready and wake up threads
    pr->rdy = 1;
    hpr1->rdy = 1;
    pthread_cond_signal(&(pr->w));
    pthread_cond_signal(&(hpr1->w));
    return h2o;
  }
  return NULL;
}

H2O *combineOxy(Oxygen *o) {
  pthread_mutex_lock(&m);
  H2O* h2o;
  // May enter only if there are 2 or more Hydrogens, to the queue otherwise
  // If there's another Oxygen before, to the queue
  if(queueLength(queues[Hy]) < 2 || !emptyQueue(queues[Ox]) || oxygen != NULL){
    h2o = enqueueO(o);
  } else {
    // If it didn't get queued, wake up Hydrogens
    oxygen = o;
    h2o = wakeup(Hy);
    oxygen = NULL;
  }
  pthread_mutex_unlock(&m);
  return h2o;
}

H2O *combineHydro(Hydrogen *h) {
  pthread_mutex_lock(&m);
  H2O* h2o;
  // Check if the molecule needs only 1 Hydrogen, if so wake up an Oxygen and Hydrogen
  if(queueLength(queues[Ox]) >= 1 && !emptyQueue(queues[Hy]) && oxygen == NULL){
    sdn_hydrogen = h;
    h2o = wakeup(Ox);
    sdn_hydrogen = NULL;
  } else {
    // Must queue until an Oxygen calls for it
    h2o = enqueueH(h); 
  }
  pthread_mutex_unlock(&m);
  return h2o;
}
