/* usar impresora con threads
 * 1 thread la puede usar a la vez
 *
 * poner modo bajo consumo si han pasado 5 mins de inactividad
 */

// se tiene:
void modoBajoConsumo();
void modoUsoNormal();


// implementar:
void obtenerImpresora();
void devolverImpresora();
void inicializarImpresora();

struct timespec{
    time_t tv_sec;
    long tv_nsec;
};

// Request a usar
typedef struct {
    int ready;
    pthread_cond_t w;
} Request;

// cola de solicitudes
Queue *q;
// mutex para seccion crit
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t obtener = PTHREAD_COND_INITIALIZER;
pthread_cond_t devolver = PTRHEAD_COND_INITIALIZER;
// bool para ocupancia de impresora
int ocupada;
pthread_t impresora;

// iniciar cola, impresora sin uso y correr impresora
void inicializarImpresora(){
    q = makeQueue();
    ocupada = 0;
    pthread_create(&impresora, NULL, ImpresoraServer, NULL)
}

void obtenerImpresora(){
    pthread_mutex_lock(&m);

    Request req = {0, PTHREAD_COND_INITIALIZER}
    put(q, &req);
    pthread_cond_signal(&obtener);

    while (!req.ready){
        pthread_cond_wait(&req.w, &m);
    }

    pthread_mutex_unlock(&m);
}

void devolverImpresora(){
    pthread_mutex_lock(&m);

    ocupada = 0;
    pthread_cond_signal(&devolver);
    pthread_mutex_unlock(&m);
}

void ImpresoraServer(){
    while(1){
        pthread_mutex_lock(&m);

        // si esta ocupada no se puede devolver
        while(ocupada)
            pthread_cond_wait(&devolver, &m);

        

        // revisar si hay threads esperando usar la impresora
        if(emptyQueue(q)){
            // si la cola esta vacia (nadie espera)
            struct  timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);

            ts.tv_nsec += 60*5; // asignar 5 minutos de timer
            while(emptyQueue(q)
                    && pthread_cond_timed_wait(&obtener, &m, &ts) != ETIMEDOUT){
                ;//wait por 5 mins
            }
            // si han pasado 5 mins o alguien espera
            if(emptyQueue(q)){
                // nadie espera
                modoBajoConsumo(); // bajo consumo
                // mientras nadie espere, esperar
                while (emptyQueue(q)){
                    pthread_cond_wait(&obtener, &m);
                }
                // llego alguien a pedir la impresora, renaudar
                modoUsoNormal();
            }
        }
        else{
            // Tomar solicitud en la cola, decirle que esta listo
            Request *p_req = get(q);
            p_req -> ready = 1;
            ocupada = 1;
            pthread_cond_signal(&p_req->w); // notificar al thread en espera
        }
        pthread_mutex_unlock(&m);
    }
}