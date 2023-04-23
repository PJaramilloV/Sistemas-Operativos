// ====================================  Sincronizacion con nThreads ====================================

/* Envia un mensaje al thread th
 * Puede ser una estructura de datos (direccion) u otra cosa
 * y suspende el thread ejecutador hasta que llegue la respuesta */
int nSend(nThread th, void *msg);


/* Suspende ejecución hasta recibir mensaje, escribe el 
 * descrpitor del thread que envió el mensaje en *pth
 * 
 * If timeout > 0 retorna despues de ese tiempo */
void *nReceive(nThread *pth, int timeout);


/* Responde a un mensaje recibido de th con 
 * el codigo de retorno rc */
void nReply(nThread th, int rc);


// IMPLEMENTADO EN A7P2.c
