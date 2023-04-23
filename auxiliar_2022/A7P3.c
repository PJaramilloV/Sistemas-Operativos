#include "nthread-impl.h"

// Lo mismo que el anterior pero ahora con timeout

/* ===================== en nthread-impl.h ===================== */
/* --------------------------- en el enum de estados --------------------------- */
/*

enum{
    READY
    (...)
    WAIT_REPLY
    WAIT_SEND
    WAIT_SEND_TIMEOUT
}

*/
/* --------------------------- en el descriptor del thread --------------------------- */

/*
struct nthread{
    (...)
    NthQueue sendQueue;
    void *msg;
}

*/

int nSend(nThread th, void *msg){
    START_CRITICAL

    // si el thread th esta en nRecieve, se pone en ready
    if(th->status == WAIT_SEND || th->status == WAIT_SEND_TIMEOUT){
        if(th->status == WAIT_SEND_TIMEOUT)
            nth_cancelThread(th);
        setReady(th);
    }


    // me
    nThread this_th = nSelf();
    // ponerme en la cola de threads que quiere mandar un mensaje
    nth_putBack(th->sendQueue, this_th);
    this_th->msg = msg; // guardar mensaje

    // esperar respuesta
    suspend(WAIT_REPLY);
    schedule();

    int rc = this_th->rc;
    // retornar rc
    END_CRITICAL
    return rc;
}

// nth_programTimer(int ns);  // suspende thread por t nanosegundos [ns]

void *nReceive(nThread *pth, int timeout_ms){
    START_CRITICAL
    nThread this_th = nSelf();

    // Si no hay mensajes en cola
    if(nth_emptyQueue(this_th->sendQueue) && timeout_ms != 0){
        if(timeout_ms > 0){
            suspend(WAIT_SEND_TIMEOUT)
            nth_programnTimer(timeout_ms * 1000000, NULL);
        } else {
            // esperar a que llegue mensaje
            suspend(WAIT_SEND);
        }
        schedule();
    }

    // tomar 1er thread de la cola y retornar su mensaje
    nThread sender_th = nth_getFront(this_th->sendQueue);
    *pth = sender_th;
    void *msg =  sender_th ? sender_th->msg :  NULL;
    //check if not NULL ~~~^ if no, sender_th->msg, si NULL entonces NULL 

    END_CRITICAL
    return msg;
}

void nReply(nThread th, int rc){
    START_CRITICAL
    
    th->rc = rc;
    setReady(th);
    scheduler();

    END_CRITICAL
}


