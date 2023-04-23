#include <nSystem.h>
#include <fifoqueues.h>

#include "reservar.h"

// marcar: asigna val a los elementos del arreglo a con indices de e
// hasta e+k-1, es decir a[e], a[e+1], ... a[e+k-1].
static void marcar(int *a, int e, int k, int val);

// buscar: encuentra k elementos consecutivos en 0 en el arreglo a de tamanno n.
// Retorna un valor e, tal que a[e]==0, a[e+1]==0, ..., a[e+k-1]==0.
// Retorna -1 si la busqueda fracasa.
static int buscar(int *a, int n, int k);

... declare aca sus estructuras y variables globales ...

void initReservar() {
  ... inicialice aca sus variables globales ...
}

int reservar(int k) {
  ... implemente reservar ...
}

void liberar(int e, int k) {
  ... implemente liberar ...
} 

// Funciones dadas

static void marcar(int *a, int e, int k, int val) {
  for (int i= e; i<e+k; i++)
    a[i]= val;
}

static int buscar(int *a, int n, int k) {
  int e= 0, i= 0, libres= 0;
  
  while(i<N_EST) {
    if (a[i]) {
      e= ++i;
      libres= 0;
    }
    else {
      libres++;
      if (libres>=k) {
        return e;
      }
      i++;
    }
  }
  return -1;
}
