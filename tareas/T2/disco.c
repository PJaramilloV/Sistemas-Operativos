#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include "disco.h"
/* Given how the problem requires a queue for the pairing of couples
 * we'll need to follow the Writer/Reader Pattern, thus a ticket 
 * system alike those of pharmacies would be in order.
 * We need a ticket dispenser for gentlemen and ladies 
 * (tick(V|D)) and a display for both (disp(V|D))
 **/ 

// Global variables
int tickV, tickD, dispV, dispD;
char *bachellor, *bachellorette;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

// Initializer of global variables
void discoInit(void) {
  tickD = 0, tickV = 0; // ticket dispensers dama and varon
  dispD = 0, dispV = 0; // ticket displays dama and varon
}

// Once done, free resources used by the mutex and cond
void discoDestroy(void) {
  pthread_mutex_destroy(&m);
  pthread_cond_destroy(&c);
}

/* For the function of getting dates whole routine is a critical section,
 * this is because we need to make sure only one pairing is made at a time.
 * The paralelism is obtained whilst dancing
 **/

// Ladies' rutine for getting their date
char *dama(char *nom) {
  pthread_mutex_lock(&m); 
  int num = tickD++;          // lady gets a ticket
  while(num != dispD)         // and waits for her turn
    pthread_cond_wait(&c, &m);
  if(bachellorette != NULL)   // if no other lady is being paired up
    pthread_cond_wait(&c, &m);
  bachellorette = nom;        // then she is now available for pairing
  pthread_cond_broadcast(&c);
  if(bachellor == NULL)       // she'll wait until a gent shows up
    pthread_cond_wait(&c, &m);
  char *gent = bachellor;     // once her partner appears, she learns his name 
  pthread_cond_broadcast(&c);
  pthread_cond_wait(&c, &m);  // waits for the gentleman to notice her
  bachellorette = NULL;       // she can prerare to leave the pairing zone
  pthread_cond_broadcast(&c); // with her dance partner
  dispV++;                    // a new gent may look for a lady, once this couple has departed
  pthread_mutex_unlock(&m);
  return gent;
}

// Gentlemens' rutine for getting their date
char *varon(char *nom) {
  pthread_mutex_lock(&m);
  int num = tickV++;          // gentleman gets a ticket
  while(num != dispV)         // and waits for his turn
    pthread_cond_wait(&c, &m);
  if(bachellor != NULL)       // if no other gent is being paired up
    pthread_cond_wait(&c, &m);
  bachellor = nom;            // then she is now available for pairing
  pthread_cond_broadcast(&c);
  if(bachellorette == NULL)   // he'll wait until a lady shows up
    pthread_cond_wait(&c, &m);
  char *lady = bachellorette; // once his partner appears, he learns her name 
  pthread_cond_broadcast(&c);
  pthread_cond_wait(&c, &m);  // waits for the lady to notice him
  bachellor = NULL;           // he can now leave the pairing zone
  pthread_cond_broadcast(&c); // with his dance partner
  dispD++;                    // a new lady may look for a gent, once this couple has departed
  pthread_mutex_unlock(&m);
  return lady;
}


/*
 * ===================== diagram ========================
 *
 * char *dama/varon(char nom){
 *  lock() -------------------- critical section init
 *  data_work
 *  while(ticket != display) ------------------ N threads up to here due to wait
 *    wait() ---------------------------------- only 1 thread from here due to 'while' condition
 *  
 *  if_1() ------------------------------------ only proceed if nobody is here
 *    wait
 *  
 *  data_work --------------------------------- say 'i am here'
 * 
 *  if_2() ------------------------------------ only proceed if 2nd thread will cooperate
 *    wait
 * 
 *  data_work --------------------------------- say 'hello there thread'
 *  wait() ------------------------------------ wait for aknowledgment
 * 
 *  data_work --------------------------------- open while for next thread
 * 
 *  unlock() ------------------ critical section end
 * }
**/