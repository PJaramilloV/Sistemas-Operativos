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

void enqueue(Hydrogen *h, Oxygen *o, Type type, H2O *pr){
  Request req = {0, h, o, NULL, PTHREAD_COND_INITIALIZER};
  put(queues[type], &req);
  while(!req.rdy)
    pthread_cond_wait(&req.w, &m);
  printf("enqueue\n");
  pr = req.water;
  printf("water assign\n");
}

void enqueueH(Hydrogen *h, H2O *pr){
  enqueue(h, NULL, Hy, pr);
}

void enqueueO(Oxygen *o, H2O *pr){
  enqueue(NULL, o, Ox, pr);
}

void wakeup(Type type, H2O *pr){
  printf("peek1\n");
  Request *pr1 = peek(queues[type]);
  Request *pr2 = NULL;
  if(pr1 == NULL){
    return;
  }
  // If Hydrogens are called to wake up
  if(type == Hy && queueLength(queues[Hy]) >= 2){
    // Set as ready, wake up and assign first Hydrogen
    pr1->rdy = 1;
    pthread_cond_signal(&pr1->w);
    firstH = pr1->h;
    get(queues[type]);
    // Set as ready, wake up and assign second Hydrogen
    printf("peek2 hydro\n");
    pr2 = peek(queues[type]);
    get(queues[type]);
    pr2->rdy = 1;
    pthread_cond_signal(&pr2->w);
    secndH = pr2->h;
    // Set back hydrogen counter
    hydros = 0;
  } 
  // Oxygen is called to wake up, by the second Hydrogen, wake up the first as well
  if(type == Ox){
    // Set as ready, wake up and assign Oxygen
    pr1->rdy = 1;
    pthread_cond_signal(&pr1->w);
    oxygen = pr1->o;
    get(queues[type]);
    // Set as ready, wake up and assign first Hydrogen
    printf("peek2 oxy\n");
    pr2 = peek(queues[!type]);
    get(queues[!type]);
    pr2->rdy = 1;
    pthread_cond_signal(&pr2->w);
    firstH = pr2->h;
    hydros = 0;
  }
  printf("make water\n");
  H2O* water = makeH2O(firstH, secndH, oxygen); 
  pr1->water = water;
  pr2->water = water;
  pr = water;
}


H2O *combineOxy(Oxygen *o) {
  pthread_mutex_lock(&m);
  H2O *water = NULL;
  // Can't enter if there aren't 2 Hydrogens, queue it
  if(hydros != 2)
    enqueueO(o, water);
  
  // when 2 Hydrogens are present this Oxygen will finish
  // the molecule, wake up the Hydrogens
  oxygen = o;
  wakeup(Hy, water);
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
  return water;
}

H2O *combineHydro(Hydrogen *h) {
  pthread_mutex_lock(&m);
  H2O *water = NULL;
  // May only enter if no Oxygens are present and current molecule
  // needs Hydrogen
  if(oxygen != NULL || hydros == 2)
    enqueueH(h, water);

  int order = 0;
  hydros++;
  // If we need another Hydrogen, queue this
  if(hydros < 2){
    order = 1;
    
    enqueueH(h, water);
    printf("2ndo enqueue H\n");
  } else {
    order = 2;
    secndH = h;
    wakeup(Ox, water);
  }

  // second Hydrogen defined before wakeup
  // first Hydrogen and Oxygen defined during wakeup
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
  printf("return water\n");
  pthread_mutex_unlock(&m);
  return water;
}
