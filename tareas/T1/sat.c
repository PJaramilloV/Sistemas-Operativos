#include <pthread.h>
#include <stdio.h>
#include <math.h>
#include "sat.h"


/* La idea es lanzar separar la ejecucion en threads
 * por cada combinacion, asi se puede tener 2 threads
 * con arreglos distintos.
 *  
 * Este proceso se debe hacer segun el p que se entrega
 * tal que sean 2^p threads, por lo tanto basta con hacer
 * 1 thread por cada combinación binaria, puesto que se
 * puede seguir en el mismo hilo donde se genera el thread
 * la otra comparacion
 **/

// Version secuencial del generador comparador
int gen_seq(int x[], int i, int n, BoolFun f) {
  int cnt = 0;
  if (i==n) {
    if ((*f)(x)){
      cnt++;
    }
  }
  else {
    x[i]= 0; 
    cnt += gen_seq(x, i+1, n, f);
    x[i]= 1; 
    cnt += gen_seq(x, i+1, n, f);
  } 
  return cnt;
}

// Crear el wrapper de argmuentos
typedef struct {
  int *x;
  int i, n, res, p;
  BoolFun f;
} Args;

// Para evitar la declaracion implicita
static int gen(int x[], int i, int n, int p, BoolFun f);

// Gen para threads, llama a gen con los argumentos del wrapper
void *gen_thread(void *pack){
  Args *args = (Args*) pack; // Castear wrapper 
  // ejecutar thread y guardar resultado en el paquete
  args->res = gen(args->x, args->i, args->n, args->p, args->f);
  return NULL;
}

// Generador comparador que crea un thread si no se ha alcanzado la
// profundidad especificada por el P, si se alcanza se sigue con la 
// version secuencial
int gen(int x[], int i, int n, int p, BoolFun f){
  if(i==p){ // Si ya no quedan threads que armar, combinar y contar
    int y[n]; 
    for(int k=0; k<p; k++) // copiar arreglo para thread
      y[k] = x[k];
    return gen_seq(y, i, n, f);
  } else { 
    
    x[i] = 0; // Establecer valor en arreglo
    int y[n]; 
    for(int k=0; k<p; k++) // copiar arreglo para thread
      y[k] = x[k];
    Args args = {y, i+1, n, 0, p, f}; // empaquetar argumentos
    pthread_t pid;  // new thread
    pthread_create(&pid, NULL, gen_thread, &args); // Lanzar thread

    x[i] = 1; // Continuar combinando
    int cnt = gen(x, i+1, n, p, f);
    pthread_join(pid, NULL); // Cerrar thread
    return cnt + args.res;  // Sumar cuentas
  }
}


// Funcion principal, lanza la funcion recursiva generadora comparadora
int recuento(int n, BoolFun f, int p) {
  int x[p]; // crear arreglo
  return gen(x, 0, n, p, f);
}
