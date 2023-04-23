#include <pthread.h>
#define P 8
// ======================== EJEMPLO BUSCAR UN FACTOR PARA UN NUMERO MUY GRANDE ===========================

typedef unsigned long long ull;
typedef unsigned int uint;

// estructura para empaquetar args del thread
typedef struct {
    ull x;
    uint i;
    uint j;
    uint res;
} Args;


// buscar factor de x en rango [i,j], si no encuentra retorna 0
uint buscarFactor(ull x, uint i, uint j){
    for(uint k=i; k <= j; k++){
        if(x%k == 0) return k;
    }
    return 0;
}

void *thread(void* p){

    Args *args = (Args*) p;
    ull x = args->x;
    uint i = args->i;
    uint j = args->j;

    args->res = buscarFactor(x,i,j);

    return NULL;
}

// buscar factor de x en rango [i,j] usando threads, si no encuentra retorna 0
uint buscarFactorParalelo(ull x, uint i, uint j){
    // Suponiendo que tenemos P cores
    pthread_t pid[P]; // crear P threads
    Args args[P]; // Crear un paquete de args para cada thread

    int intervalo = (j-i)/P;
    for(uint k = 0; k < P; k++){
        args[k].x = x;
        args[k].i = i + intervalo*k;
        args[k].j = i + intervalo*(k+1) -1; // -1 para evitar que se incluya el inicio del siguiente intervalo

        pthread_create(&pid[k], NULL, thread, &args[k]);
    }

    // valor predeterminado del factor
    uint factor = 0;
    for(uint k = 0; k < P; k++){

        pthread_join(pid[k], NULL); // no guardar el valor de retorno puesto que esta guardado en el paquete
        if(args[k].res != 0){factor = args[k].res;} // Si hay factor, actualizar factor

    }

    return factor;
}