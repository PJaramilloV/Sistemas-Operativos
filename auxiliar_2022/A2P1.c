// quicksort con pthreads
#include <pthread.h>
#define n 2

// version sequencial
void quicksort_seq(int a[], int i, int j){
    if(i < j){
        int h = particionar(a,i,j);
        quicksort_seq(a, i, h-1); // paralelize this
        quicksort_seq(a, h+1, j); // this shit
    }
}



// estructura de datos para pasar argumentos al pthread
typedef struct {
    int a[];
    int i, j, n;
} Args;



// funcion a pasarle al pthread
void *thread_func(void *p){
    Args *args = (Args*) p;
    quicksort(args->a, args->i, args->j, args-> n );
    return NULL;
}

// quicksort usando N cores
void quicksort(int a[], int i, int j, int n){
    if(n == 1) // CASO BASE: se dividen cores hasta llegar a 1, en este caso proseguimos de forma secuencial
        quicksort_seq(a, i, j);
    else{
        if(i < j){
            int h = particionar(a,i,j); // calcular pivote

            // paralelizar 1ra llamada recursiva
            pthread_t pid;                                  //crear thread
            Args args = {a, i, h-1, n/2};                   // almacenar args
            pthread_create(&pid, NULL, thread_func, &args);  //

            // 2da llamada recursiva
            quicksort(a, h+1, j, n - n/2);  // asignar el resto de los cores: n - n/2

            // cerrar thread
            pthread_join(pid, NULL);
        }
    }
}