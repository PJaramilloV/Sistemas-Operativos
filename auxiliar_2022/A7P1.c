// Resolver problema de impresora del Aux 5, usando mensajes

enum Mensaje {OBTENER, DEVOLVER};

void obtenerImpresora(){
    int msg = OBTENER;
    nSend(impresora, &msg); // Bloquear thread hasta recibir respuesta
}

void devolverImpresora(){
    int msg = DEVOLVER;
    nSuspend();
}

// ignorar argumento
int impresoraServer(void *_ignored){
    Queue *q = makeQueue();
    int ocupado = FALSE;

    nThread t;
    int *msg;

    while (TRUE){
        if(!ocupado){
            msg = (int *) nReceive(&t, 60 * 5 * 1000);
            if(t == NULL){
                modoBajoConsumo();
                msg = (int*) nRecieve(&t, -1);
                modoUsoNormal();
            }
        } else {
            msg = (int*) nRecieve(&t, -1);
        }
        if(*msg == OBTENER){
            if(ocupado)
                put(q,t);
            else{
                ocupado = TRUE;
                nReply(t,0);
            }
            
        } else if( *msg == DEVOLVER){
            nReply(t, 0);
            if( EmptyFifoQueue(q))
                ocupado = FALSE;
            else {
                nThread *t2 = (nThread*) get(q);
                nReply(t2, 0);
            }
        }
    }
    
}

