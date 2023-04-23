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
Oxygen* oxygen;
Queue* queues[2];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync = PTHREAD_COND_INITIALIZER;

void initH2O(void) {
  queues[0] = makeQueue(); 
  queues[1] = makeQueue();  
}

void endH2O(void) {
  destroyQueue(queues[Hy]);
  destroyQueue(queues[Ox]);
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

void enqueueO(Oxygen *o){
  enqueue(NULL, o, Ox);
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
    if(hpr2 == NULL){
      printf("extraccion nula\n");
      }
    if((hpr2->h) == NULL){
      printf("hidrogeno nulo\n");
      }
    get(queues[Hy]);
    // Get oxygen from global var and reset global var
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
    pr->rdy = 1;
    pthread_cond_signal(&(pr->w));
  }
  return NULL;
}

H2O *combineOxy(Oxygen *o) {
  pthread_mutex_lock(&m);
  // May enter only if there are 2 or more Hydrogens, queue it otherwise
  // If there's another Oxygen before, queue it
  if(queueLength(queues[Hy]) < 2 || !emptyQueue(queues[Ox]) || oxygen != NULL){
    enqueueO(o);
  }
  
  oxygen = o;
  // Once unqueued wake up Hydrogens
  H2O* h2o = wakeup(Hy);
  oxygen = NULL;
  pthread_mutex_unlock(&m);
  return h2o;
}

H2O *combineHydro(Hydrogen *h) {
  pthread_mutex_lock(&m);
  // Check if an oxygen needs waking
  if(queueLength(queues[Ox]) >= 1 && !emptyQueue(queues[Hy]) && oxygen == NULL){
    wakeup(Ox);
    }
  // Must queue until an Oxygen calls for it
  H2O* h2o = enqueueH(h);
  pthread_mutex_unlock(&m);
  return h2o;
}
