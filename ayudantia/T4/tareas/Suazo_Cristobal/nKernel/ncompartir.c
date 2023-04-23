#define _XOPEN_SOURCE 500
#include "nthread-impl.h"

// Use los estados predefinidos WAIT_ACCEDER y WAIT_COMPARTIR
// El descriptor de thread incluye el campo ptr para que Ud. lo use
// a su antojo.

//... defina aca sus variables globales con el atributo static ...
NthQueue *colaWaitnTh; //Para guardar los nThreads que usen nAcceder antes de que
//otro nThread use nCompartir
nThread nCom; //Para saber quién es el thread que inició nCompartir
int contador; //Para saber cuánto threads hay en la cola
 

// nth_compartirInit se invoca al partir nThreads para Ud. inicialize
// sus variables globales

void nth_compartirInit(void) {
  contador = 0;
  nCom = NULL; //no hay ningún conchetumare >:C
  colaWaitnTh = nth_makeQueue();
}

void nCompartir(void *ptr) {
  
  START_CRITICAL
  //Guardamos el id del nThread en nthComp
  nThread nthComp = nSelf();
  //Le entregamos el puntero que recibimos
  nthComp->ptr = ptr;
  //Finalmente lo almacenamos en la variable global
  nCom  = nthComp;
  //Si la cola no está vacia, iteramos en el ciclo while
  while(!nth_emptyQueue(colaWaitnTh)) { 
    //Sacamos el primer nThread que usó nAcceder
    nThread temp = nth_getFront(colaWaitnTh); 
    contador++;
    //Le entregamos el ptr, para poder reconocerlo
    temp->ptr=ptr; 
    setReady(temp);
  }
  //Si la cola está vacía, simplemente se ejecuta la siguiente linea de código
  suspend(WAIT_COMPARTIR);
  schedule();
  END_CRITICAL
  return;
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  //instanciamos el nThread
  nThread nthAcc = nSelf();
  /*Si algún thread no ha llamado a nCompartir, entonces
  Hay que dejarlos en una cola, hasta que se haga el llamado
  */
  if(nCom==NULL) {
    //agregamos el nThread a la cola
    nth_putBack(colaWaitnTh,nthAcc);
    //Lo dejamos esperando hasta que los despierten
    suspend(WAIT_ACCEDER);
    schedule();
  }
  //Si ya hay un llamado de nCompartir, cualquier nThread que llame
  //a nAcceder, debe retornar de inmediato
  
  void * puntero; 
  //Este if es para contar los nThreads que llamen a nAcceder
  //después de que se haya hecho la llamada de nCompartir
  if(nthAcc->ptr!=nCom->ptr){
    //Si el ptr es distinto que en la variable global, significa
    //que no lo hemos contado, por lo tanto hay que aumentar
    //el contador, y marcarlos per se.
    contador++;
    nthAcc->ptr = nCom->ptr;
  }
  //en Este punto, sabemos que cualquier llamado, tiene asignado
  //el campo ptr, por lo que no podemos asignar este valor en puntero
  puntero = nthAcc->ptr;
  END_CRITICAL
  //Aquí hay que retornar el puntero
  return puntero;
}

void nDevolver(void) {
  START_CRITICAL
  nThread nthDev = nSelf();
  //Cuando el contador llega a cero, es porque ya no quedan
  //nThreads en la q_wait_nth
  contador--;
  //Como ya retornó, le asignamos el puntero como NULL
  nthDev->ptr=NULL;
  if(contador==0){ 
    //Ya no quedan mas nThreads en la llamada de nCompartir.
    nCom->ptr=NULL;   
    setReady(nCom); //Ahora pasamos a estado READY nCom, para que pueda retornar
    nCom = NULL;
    schedule();
  }
  END_CRITICAL
}
