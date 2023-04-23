#include <pthread.h>

#include "pss.h"
#include "h2o.h"

//.. define aca las estructuras de datos que necesite ...
typedef enum {Hy, Ox} Type; 

typedef struct {
  int rdy;
  Hydrogen* h;
  Oxygen* o;
  pthread_cond_t w;
} Request;


//.. define aca las variables globales que necesite ...
Hydrogen* firstH;
Hydrogen* secndH;
Oxygen*   oxygen;
Queue* queues[2];
int hydros, h1rdy, h2rdy, oxyrdy, go;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sync = PTHREAD_COND_INITIALIZER;


void initH2O(void) {
  hydros = 0; go = 0;
  queues[0] = makeQueue(); 
  queues[1] = makeQueue();
}

void endH2O(void) {
  destroyQueue(queues[Hy]);
  destroyQueue(queues[Ox]);
}

/* Idea:
 * Base off solution from the WRITER/READER template
 * where readers are Hydrogens and writers are Oxygens
 * but with a twist: make it so the Hydrogens are the
 * "writing material" so in terms of the access of threads we have:
 * 
 * an Oxygen may only enter if there are 2 Hydrogens, if else it'll have to wait
 * a Hydrogen may only enter if there are 0 Oxygens and 0-1 Hydrogens, if else it'll have to wait
 * */

void enqueue(Hydrogen *h, Oxygen *o, Type type){
  Request req = {0, h, o, PTHREAD_COND_INITIALIZER};
  put(queues[type], &req);
  while(!req.rdy)
    pthread_cond_wait(&req.w, &m);
}

void enqueueH(Hydrogen *h){
  enqueue(h, NULL, Hy);
}

void enqueueO(Oxygen *o){
  enqueue(NULL, o, Ox);
}

void wakeup(Type type){
  Request *pr = peek(queues[type]);
  if(pr == NULL){
    return;
  }
  // If Hydrogens are called to wake up
  if(type == Hy && hydros == 2){
    // Set as ready, wake up and assign first Hydrogen
    pr->rdy = 1;
    pthread_cond_signal(&pr->w);
    firstH = pr->h;
    get(queues[type]);
    // Set as ready, wake up and assign second Hydrogen
    Request *pr = peek(queues[type]);
    get(queues[type]);
    pr->rdy = 1;
    pthread_cond_signal(&pr->w);
    secndH = pr->h;
    // Set back hydrogen counter
    hydros = 0;
  } 
  // Oxygen is called to wake up, by the second Hydrogen, wake up the first as well
  if(type == Ox){
    // Set as ready, wake up and assign Oxygen
    pr->rdy = 1;
    pthread_cond_signal(&pr->w);
    oxygen = pr->o;
    get(queues[type]);
    // Set as ready, wake up and assign first Hydrogen
    Request *pr = peek(queues[!type]);
    get(queues[!type]);
    pr->rdy = 1;
    pthread_cond_signal(&pr->w);
    firstH = pr->h;
    hydros = 0;
  }
}


H2O *combineOxy(Oxygen *o) {
  pthread_mutex_lock(&m);
  // Can't enter if there aren't 2 Hydrogens, queue it
  if(hydros != 2)
    enqueueO(o);
  
  // when 2 Hydrogens are present this Oxygen will finish
  // the molecule, wake up the Hydrogens
  oxygen = o;
  wakeup(Hy);
  Hydrogen *h1 = firstH;
  Hydrogen *h2 = secndH;
  oxyrdy = 1;

  go = 0;
  // Wait for all three atoms to awknowledge eachother
  while(!(h1rdy == 1 && h2rdy == 1 && oxyrdy == 1) && go == 0)
    pthread_cond_wait(&sync, &m);
  
  // First thread to pass condition makes 2 tickets
  if(go == 0){
    go = 2;
  } else {
    // Consume ticket
    go--;
  }
  pthread_cond_broadcast(&sync);
  firstH = NULL;
  secndH = NULL;
  oxyrdy = 0;
  pthread_mutex_unlock(&m);
  return makeH2O(h1, h2, o);
}

H2O *combineHydro(Hydrogen *h) {
  pthread_mutex_lock(&m);
  // May only enter if no Oxygens are present and current molecule
  // needs Hydrogen
  if(oxygen != NULL || hydros == 2)
    enqueueH(h);

  int order = 0;
  hydros++;
  // If we need another Hydrogen, queue this
  if(hydros < 2){
    order = 1;
    enqueueH(h);
  } else {
    order = 2;
    secndH = h;
    wakeup(Ox);
  }

  // second Hydrogen defined before wakeup
  // first Hydrogen and Oxygen defined during wakeup
  Hydrogen *h1 = firstH;
  Hydrogen *h2 = secndH;
  Oxygen *o = oxygen;
  if(order == 1){
    h1rdy = 1;
  } 
  if(order == 2){
    h2rdy = 1;
  }

  go = 0;
  // Wait for all three atoms to awknowledge eachother
  while(!(h1rdy == 1 && h2rdy == 1 && oxyrdy == 1) && go == 0)
    pthread_cond_wait(&sync, &m);
  
  // First thread to pass condition makes 2 tickets
  if(go == 0){
    go = 2;
  } else {
    // Consume ticket
    go--;
  }
  pthread_cond_broadcast(&sync);
  oxygen = NULL;
  h1rdy = 0;
  h2rdy = 0;
  pthread_mutex_unlock(&m);
  return makeH2O(h1, h2, o);
}
