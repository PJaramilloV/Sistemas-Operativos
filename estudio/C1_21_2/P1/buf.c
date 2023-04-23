#include <nSystem.h>
#include "buf.h"

int buf;    // El buffer de tamaño 1
nSem prod;  // = nMakeSem(0);
nSem cons;  // = nMakeSem(0);

void initBuf() {
  prod= nMakeSem(0);
  cons= nMakeSem(0);
}

void put(int item) {
  buf= item;
  nSignalSem(prod);
  nWaitSem(cons);
}

// ==========================================================
// Solo puede modificar a partir de aca
//

int get() {
  nWaitSem(prod);
  nSignalSem(cons);
  return buf;
}
