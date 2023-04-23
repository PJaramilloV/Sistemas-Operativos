/** Los threads en C son los pthreads, estos comparten memoria entre los threads a diferencia de los padres e hijos de los procesos pesados de PSS donde se tenia que asignar muchas direcciones de memoria
 * 
 * Como crear un thread? --------------------------------------------------------------------------------
 * 
 * int pthread_create(pthread_t *thread,                   // id del proceso thread
 *                    const pthread_attr_t *attr,          // atributos especiales para la creacion del thread
 *                    void *(*start_routine) (void *),     // funcion a ejecutar
 *                    void *arg                            // parametro de func
 *                    );
 * 
 *      pthread_create -> 0 si es exitoso, != 0 si algo falló
 * 
 * 
 * Como matar un thread? --------------------------------------------------------------------------------
 * 
 * si la funcion "start_routine"(3er arg) retorna, o también llamando a "void pthread_exit(void* return_value)"
 *      esto es DISTINTO A exit(), un thread NO termina con exit()
 * 
 * 
 * Esperar termino de thread --------------------------------------------------------------------------------
 * 
 * alguien debe esperar el termino de pthread_create, similar a wait_pid
 * esto se hace con 
 * 
 *      int pthread_join(pthread_t thread,      // id de thread a esperar
 *                       void **return_value    // puntero a puntero de lugar de memoria para guardar resultado
 *                      );
 * 
 *          retorna 0 si es exitoso, si un thread no se entierra se vuelve zombie, como un proceso pesado
 */

// ================================= EJEMPLO BASICO DE THREAD =================================

#include <stdio.h>
#include <pthread.h>

// func thread recibe un puntero, a este se le castea como un puntero string y se printea, excepto si dice 'weon aweonado'
void *thread(void *ptr){
    char* nombre = (char*) ptr;

    if(*nombre == "weon aweonado"){
        phtread_exit(NULL);
    }
    printf("Thread - %s\n", nombre);

    return NULL;
}

int main(){

    pthread_t pid_1, pid_2;
    char* nombre_1 = "primero";
    char* nombre_2 = "segundo";

    pthread_create(&pid_1, NULL, thread, nombre_1);  // Sin params especiales para manejar el thread
    pthread_create(&pid_2, NULL, thread, nombre_2);

    pthread_join(pid_1, NULL);  // no guardar resultado
    pthread_join(pid_2, NULL);

    return 0;
}



/**
 * Pero que pasa si queremos threads con más de 1 arg para el thread?
 * 
 * crear una estructura de datos que reuna los args en un solo obj y luego entregar esa
 */

