// Implementar Lector / Escritor usando Spin Locks


void enterRead(){
    spinLock(&mutex);
    if (readers == 0)
        spinLock(&writeLock);
    
    readers++;
    spinUnlock(&mutex);
}

void enterWrite(){
    spinLock(&writeLock);
}

void exitRead(){
    spinLock(&mutex);
    readers--;
    if(readers == 0)
        spinUnlock(&writeLock);
    spinUnlock(&mutex);
}

void exitWrite(){
    spinUnlock(&writeLock);
}