// detector de plagio
// revisa que tanto se parecen 2 tareas de alumnos

// version sequencial
void masParecidasSeq(Tarea *tareas, int n, int *pi, int *pj){
    int min=MAXINT;
    for(int i=0; i<n ; i++){
        for(int j=0; j<i; j++){
            int similitud = compTareas(tareas[i], tareas[j]);
            if(similitud < min){
                min = similitud;
                *pi=i; 
                *pj=j;
            }
        }
    }
}

// implementar version // para 8 cores
// patron prodcutor/consumidor de Jobs
#define P 8
#define BUFFERSIZE 128
typedef Tarea;
typedef struct {
    Tarea *tareas;
    int i, j;
    int *pi, *pj;
    int *min;
    pthread_mutex_t *pm;
} Job;


// consumidor
void thread_func(void *ptr){
    Buffer *buf = ptr;

    while(1){
        Job *p_job = bufGet(buf);
        if(p_job = NULL){
            return NULL;
        }
        int similitud = compTareas(p_job->tareas[p_job->i], p_job->tareas[p_job->j]);

        pthread_mutex_lock(p_job->pm);
        if(similitud < (p_job->min)){
            *(p_job->min) = similitud;
            *(p_job->pi)= (p_job->i); 
            *(p_job->pj)= (p_job->j);
        }
        pthread_mutex_unlock(p_job->pm);

        free(p_job);
    }
}

// productor
void masParecidas(Tarea *tareas, int n, int *pi, int *pj){
    int min = MAXINT;
    pthread_mutex_t m = PHREAD_MUTEX_INITIALIZER;

    Buffer *buf = makeBuffer(BUFFERSIZE);

    pthread_t pid[P];

    for(int p=0; p< P; p++){
        pthread_create(&pid[p], NULL, thread_func, buf);
    }

    for(int i=0; i<n ; i++){
        for(int j=0; j<i; j++){

            Job job = {tareas, i, j, pi, pj, &min, &m};
            Job *p_job = malloc(sizeof(Job));
            *p_job = job;

            putBuf(buf, p_job);
        }
    }

    for(int p=0; p < P; ++p){
        put(buf, NULL);
    }
    for(int p=0; p<P; ++p){
        pthread_join(pid[k], NULL);
    }
    destroyBuffer(buf);
}