#include <nSystem.h>

#include "buf.h"

static int producer(int iter) {
  nSetTaskName("producer");
  while (iter--) {
    put(1);
    put(2);
  }
  return 0;
}

static int consumer(int iter) {
  nSetTaskName("consumer");
  while (iter--) {
    int item1= get();
    int item2= get();
    if (item1!=1 || item2!=2)
      nFatalError("consumer", "Valores incorrectos: %d %d\n", item1, item2);
  }
  return 0;
}

#define ITER 10000

int nMain() {
  initBuf();
  nTask prod= nEmitTask(producer, ITER);
  nTask cons= nEmitTask(consumer, ITER);
  nWaitTask(prod);
  nWaitTask(cons);

  nPrintf("Felicitaciones: aprobo el test\n");
  return 0;
}
