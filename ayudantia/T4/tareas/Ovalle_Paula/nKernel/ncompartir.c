#define _XOPEN_SOURCE 500

#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...

//variable para contar el numero de threads que han accesido a los datos
//se usara para que compartir sepa cuando retornar
static int count_threads;
static nThread *thread_sharing;
static NthQueue *acceso_threads;

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  //...
  count_threads = 0;
  thread_sharing = NULL;
  acceso_threads = nth_makeQueue();
  
}

void nCompartir(void *ptr) {
  
  START_CRITICAL;
  nThread thisTh= nSelf();
  thread_sharing = &thisTh;
  //thread_sharing = setSelf();
  (*thread_sharing)->ptr = ptr;
  
  
  
  //if (!nth_emptyQueue(acceso_threads)) {
  //    //suspend(WAIT_COMPARTIR);
   //   while (!nth_emptyQueue (acceso_threads)){
     //     nThread th_acceso = nth_getFront(acceso_threads);
          //nThread th_acceso = nth_peekFront(acceso_threads);
       //   if (th_acceso->status == WAIT_ACCEDER){
         //     setReady(th_acceso);
          //}
      
      
     // }
  //    schedule();
  
  
  
  // if (!((*thread_sharing)->status == READY)){
  
     while (!nth_emptyQueue (acceso_threads)){
     
        nThread th_acceso = nth_getFront(acceso_threads);
        // if (th_acceso->status == WAIT_ACCEDER && !(th_acceso->status == READY) && !(th_acceso->status == RUN) ){
            setReady(th_acceso);
        // }
      }
      // schedule();
      
  // }
  
  // if (count_threads >= 0){
  
     suspend(WAIT_COMPARTIR);
  // } 
  
  schedule();
  
  
  
  //thread_sharing = NULL;

  END_CRITICAL;
  
  return;
}

void *nAcceder(int max_millis) {
  
  START_CRITICAL;
  
  count_threads++;
  
  nThread thisTh= nSelf();
  
  //nth_putBack(acceso_threads,thisTh);
  
  
  if (thread_sharing == NULL){
     nth_putBack(acceso_threads,thisTh);
     suspend(WAIT_ACCEDER);
  }
  
  schedule();
  
  //nth_getFront(acceso_threads);
  void *pointer = (*thread_sharing)->ptr;
  END_CRITICAL;
 
  //void *pointer = (*thread_sharing)->ptr;
  return pointer;
  
  
}

void nDevolver(void) {
  START_CRITICAL;
  count_threads--;
  if (count_threads == 0){
     // if ((*thread_sharing)->status == WAIT_COMPARTIR){
        setReady(*thread_sharing);
        thread_sharing = NULL;
     
     // }
     schedule();
  }
  
  END_CRITICAL;
  
  
  return;
}
