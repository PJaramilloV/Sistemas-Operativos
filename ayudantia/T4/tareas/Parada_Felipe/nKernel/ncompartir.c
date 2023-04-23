#define _XOPEN_SOURCE 500
/*
    CC4302 Sistemas Operativos – Semestre primavera 2022 – Tarea 4 – Prof.: Luis Mateu
    Autor: Felipe Parada B.
*/
#include "nthread-impl.h"
#include <stdlib.h>
#include <stdio.h>

// Se definen las variables globales
static NthQueue* cola; // Cola para guardar nThreads que invocaron nAcceder y esperan nCompartir
static int esperando = 0; // Indica el número de nThreads que invocaron nAcceder y que no han invocado nDevolver
static int compartiendo = 0; // Indica si hay un nThread que invocó nCompartir (0 si no hay)
static nThread comparte_th; // nThread para guardar al que invocó nCompartir

void nth_compartirInit(void) {
  cola = nth_makeQueue(); // Se inicializa el nth_Queue cola 
}

void nCompartir(void *ptr) {
  START_CRITICAL
  comparte_th = nSelf(); // Se guarda a sí mismo como el nThread que está compartiendo
  comparte_th -> ptr = ptr; // Se gurda el ptr en el descriptor del nThread
  compartiendo = 1; // Se indica que hay un nThread que invocó nCompartir

  while (!nth_emptyQueue(cola)){ // Mientras la cola no esté vacía
    nThread accede_th = nth_getFront(cola); // Se saca el primer nThread de la cola
    accede_th -> ptr = ptr; // Se le guarda ptr en el descriptor
    setReady(accede_th); // Ese nThread se marca como READY
  }

  suspend(WAIT_COMPARTIR); // Se espera a que el último nThread que invocó nAcceder invoque nDevolver
  schedule();
  END_CRITICAL
  return;
}

void *nAcceder(int max_millis) {
  START_CRITICAL
  ++esperando; // Se indica que hay un nThread más que invocó nAcceder

  if(compartiendo){ // Si hay un nThread que invocó nCompartir y todavía no retorna
    return comparte_th->ptr; // Devuelve lo que el nThread que está compartiendo tienen guardado en ptr
  }
  
  // Si NO hay un nThread que invocó nCompartir y todavía no retorna
  nth_putBack(cola, nSelf()); // Se guarda este nThread en la cola
  suspend(WAIT_ACCEDER); // Se espera a que un nThread invoque nCompartir
  schedule();
  END_CRITICAL
  nThread este_th = nSelf();  // Se rescata este nThread
  return este_th->ptr; // Se retorna lo que este nThread ahora tiene en ptr
}

void nDevolver(void) {
  START_CRITICAL
  --esperando; // Se indica que el nThread ya invocó nDevolver
  if (esperando == 0){ // Si este nThread es el último en invocar nDevolver
    compartiendo = 0; // Se indica que ya no hay un nThread compartiendo
    setReady(comparte_th); // Se le indica al nThread que esta compartiendo que ya puede retornar
    comparte_th = NULL; // Se deja nulo el nThread donde se guarda al nThread que invocó nCompartir
  }
  END_CRITICAL
  return;
}
