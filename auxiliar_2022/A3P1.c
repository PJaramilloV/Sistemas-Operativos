/* Patron Request
 * 
 * Cada thread tiene su propia condicion para asi evitar 
 * despertar a todos los threads e ir llamando espeficicos 
 * threads a despertarse cuando corresponde
**/

/* 
 *
 *
 *
**/
#include<pthread.h>

typedef struct {
    int a;
}Camion;

typedef struct {
    int a;
}Ciudad;

typedef struct {
    int a;
}Contenedor;

pthread_mutex_t m;
Camion *c;
Ciudad *ubic= &Stgo;

void transportar(Contenedor *cont, Ciudad *orig, Ciudad *dest){
    pthread_muted_lock(&m);
    conducir(c, ubic, orig);
    cargar(c, cont);
    conducir(c, orig, dest);
    descargar(c, cont);
    ubic = dest;
    pthread_mutex_unlock(&m);
}

// Sea condicur, cargar y descargar funcs que toman mucho tiempo,
// transportar espera cuando el unico camion de la empresa esta ocupado
// antes de cargar el camion hay que condicir desde origen


// ahora tenemos 8 camiones

#define P 8
#define TRUE 1
#define FALSE 0

Camion *camiones[P];
Ciudad *ubic[P];
double distancia(Ciudad * orig, Ciudad *dest); // dest entre 2 ciudades

// en t0 todos los camiones estan en Stgo
//
// Restricciones:
//
// Un camion solo se puede usar en 1 transporte a la vez
// Un camion no puede permanecer ocioso si hay algo que hacer
// Si al invocar transportar y hay varios camiones disponibles,
//      responde el camion mas cercano al origen del container
// Si un camion se desocupa y hay varias tareas a hacer, se
//      debe atender la mas cercana al camion

// se asume que hay un numero max de tareas pendientes
#define R 100 // numero max de solicitudes

int ocupados[P];  // ocupado[k] = T; si el camion esta ocupado

typedef struct {
    int ready, idx;
    pthread_cond_t c;
    Ciudad *orig;
} Request;

Request *requests[R];

int buscar(Ciudad *orig){
    int idx = -1;
    // hay camiones desocupados, se elige el mas cercano
    for(int k=0; k<P; k++){
        if(!ocupados[k]){
            // Si es el primer camion, o esta mas cerca que el camion candidato
            // entonces este camion es mi nuevo camion candidato
            if(idx == -1 || distancia(ubic[k], orig) < distancia(ubic[idx], orig)){
                idx = k;
            }
        }
    }
    // no hay camiones desocupados -> esperar (REQUEST)
    if(idx < 0){
        Request req = {FALSE, -1, orig, PTHREAD_COND_INITIALIZER};

        for(int i=0; i<R; i++){ // guardar request
            if(requests[i] == NULL){
                requests[i] = &req;
                break;
            }
        }
        // esperar request hasta que se atienda
        while (!req.ready){
            pthread_cond_wait(&req.c, &m);
        }
        idx = req.idx; // This solo si se despierta el request
    }
    return idx;
}

void desocupar(int idx){
    // Si hay solicitudes pendientes, elegir el contenedor mas cercano
    int req_idx = -1;
    for(int i=0; i<R; i++){
        if(requests[i] != NULL){
            if(req_idx == -1 || 
                distancia(requests[i]->orig, ubic[idx]) < distancia(ubic[idx], requests[req_idx])){
                req_idx = i;
            }
        }
    }
    if(req_idx < 0){
        ocupados[idx] = FALSE;
    } else {
        requests[req_idx]->ready = TRUE;
        requests[req_idx]->idx = idx;
        pthread_cond_signal(&requests[req_idx]->c);
        requests[req_idx] = NULL;
    }
    //si no hay solicitudes, desocupar el camion
}

void transportar(Contenedor *cont, Ciudad *orig, Ciudad *dest){
    pthread_muted_lock(&m); 
    
    // Buscar la el camion mas cercano (segun ubic) al origen
    int idx = buscar(orig, ubic); // retorna indice del camion asignado
    ocupados[idx] = TRUE;
    Camion *c = camiones[idx];
    pthread_mutex_unlock(&m);

    conducir(c, ubic[idx], orig);
    cargar(c, cont);
    conducir(c, orig, dest);
    descargar(c, cont);

    pthread_muted_lock(&m);
    ocupados[idx] = FALSE;
    ubic[idx] = dest;
    desocupar(idx);
    pthread_mutex_unlock(&m);
    
}




















