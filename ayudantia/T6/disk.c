#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "disk.h"
#include "priqueue.h"
#include "spinlocks.h"

// Le sera de ayuda la clase sobre semáforos:
// https://www.u-cursos.cl/ingenieria/2022/2/CC4302/1/novedades/detalle?id=431689
// Le serviran la solucion del productor/consumidor resuelto con el patron
// request y la solucion de los lectores/escritores, tambien resuelto con
// el patron request.  Puede substituir los semaforos de esas soluciones
// por spin-locks, porque esos semaforos almacenan a lo mas una sola ficha.


typedef struct {
int *sl;
int track;
} Request;

// Declare aca las variables globales que necesite
int slm; //Spinlock que sirve de mutex
PriQueue * colamenor;
PriQueue * colamayor;

int ocupado; //OPEN es 0, CLOSEd es 1
int ultimapista;

// Agregue aca las funciones requestDisk y releaseDisk

void iniDisk(void) {
  slm=OPEN;
  colamenor= makePriQueue();
  colamayor= makePriQueue();
  ocupado=OPEN;
}

void requestDisk(int track) {
  spinLock(&slm);
  if(ocupado==OPEN){
    ocupado=CLOSED;
    ultimapista=track;
    spinUnlock(&slm);
    return;
  }
  else{
    int mysl=CLOSED;
    Request req={&mysl,track};
    if(ultimapista<=track){
      priPut(colamayor,&req,track);
    }
    else{
      priPut(colamenor,&req,track);
    }
    spinUnlock(&slm);
    spinLock(&mysl);
    return;
  }
}

void releaseDisk() {
  spinLock(&slm);
  if(emptyPriQueue(colamayor) && emptyPriQueue(colamenor)){
    ocupado=OPEN;
    spinUnlock(&slm);
    return;
  }
  else{
    if(!emptyPriQueue(colamayor)){
      Request *modificar=priGet(colamayor);
      ocupado=CLOSED;
      ultimapista=modificar->track;
      spinUnlock(modificar->sl);
      spinUnlock(&slm);
      return;
    }
    else{
      /*
      colamayor=colamenor;
      colamenor=makePriQueue();
      */
      while(!emptyPriQueue(colamenor)){
        Request *poner=priGet(colamenor);
        priPut(colamayor,poner,poner->track);
      }
      Request *modificar=priGet(colamayor);
      ocupado=CLOSED;
      ultimapista=modificar->track;
      spinUnlock(modificar->sl);
      spinUnlock(&slm);
      return;
    }
  }
}
