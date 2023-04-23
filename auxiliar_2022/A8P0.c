// Auxiliar de Spin-Locks
// herramientas de sincronización que usan busy waiting

// Crear
int lk = OPEN;

// LOCK
spinLock(&lk);

// Unlock

spinUnlock(&lk);