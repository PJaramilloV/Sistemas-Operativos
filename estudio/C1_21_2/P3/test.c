#include <nSystem.h>

// Para poder usar sprintf:
#include <stdio.h>

#include "reservar.h"

#define N_EST 10

static int ocup[N_EST];
static nMonitor m;
static int verbose= TRUE;
static int tiempo_actual;

//===================================================================
// Manejo de pausas
//

static nMonitor t_mon;

void iniciar() {
  nEnter(t_mon);
  tiempo_actual= 1;
  nExit(t_mon);
}

int tiempoActual() {
  nEnter(t_mon);
  int t= tiempo_actual;
  nExit(t_mon);
  return t;
}

void pausa(int tiempo_espera) { /* tiempo_espera en centesimas de segundo */
  nEnter(t_mon);
  int tiempo_inicio= tiempo_actual;
  nExit(t_mon);
  nSleep(tiempo_espera*300);
  nEnter(t_mon);
  tiempo_actual= tiempo_inicio+tiempo_espera;
  nExit(t_mon);
}

//===================================================================
// Manejo de vehiculos
//

typedef struct {
  nTask t;
  char *nom;
  int t_reserva;
} Vehiculo;
 
int vehiculoFun(Vehiculo *v, char *iter, int k, int tiempo) {
  nSetTaskName(v->nom);
  while (iter--) {
    if (verbose)
      nPrintf("%d: %s solicita %d estacionamientos\n",
              tiempoActual(), v->nom, k);
    int e= reservar(k);
    v->t_reserva= tiempoActual();
    for (int j= e; j<e+k; j++) {
      if (j>=N_EST)
        nFatalError("estacionarVehiculo", "estacionamiento %d no existe\n",
                    j);
      if (ocup[j])
        nFatalError("estacionarVehiculo", "estacionamiento %d esta ocupado\n",
                    j);
      ocup[j]= TRUE;
    }
    if (verbose)
      nPrintf("%d: %s reserva %d estacionamiento desde %d\n",
              tiempoActual(), v->nom, k, e);
    if (tiempo>0)
      pausa(tiempo);
    if (verbose)
      nPrintf("%d: %s libera %d estacionamientos desde %d\n",
              tiempoActual(), v->nom, k, e);
    for (int j= e; j<e+k; j++) {
      ocup[j]= FALSE;
    }
    liberar(e, k);
  }
  return 0;
}

Vehiculo *estacionarVehiculo(char *nom, int iter, int k, int tiempo) {
  Vehiculo *v= nMalloc(sizeof(Vehiculo));
  v->nom= nom;
  v->t= nEmitTask(vehiculoFun, v, iter, k, tiempo);
  return v;
}

void esperarVehiculo(Vehiculo *v, int t_reserva) {
  nWaitTask(v->t);
  if (t_reserva>0 && t_reserva!=v->t_reserva)
    nFatalError("esperarVehiculo",
                "Tiempo de otorgamiento de la reserva erroneo de %s. "
                "Es %d y debio ser %d.\n", v->nom, v->t_reserva, t_reserva);
  nFree(v);
}

//===================================================================
// Test unitarios
//

void testUnitarios() {
  iniciar();
  Vehiculo *renault, *mercedes, *chevrolet, *suzuki, *toyota, *mg, *ford, *bmw;
  nPrintf("--- Test: un solo vehiculo no espera -------------------------\n");
  renault= estacionarVehiculo("renault", 1, 2, 0);
  esperarVehiculo(renault, 1);
  
  nPrintf("--- Test: se ocupan todos los estacionamientos, "
          "pero nadie espera ---\n");
  iniciar();
  renault= estacionarVehiculo("renault", 1, 2, 3);
  pausa(1);
  mercedes= estacionarVehiculo("mercedes", 1, 3, 2);
  pausa(1);
  chevrolet= estacionarVehiculo("chevrolet", 1, 5, 1);
  esperarVehiculo(renault, 1);
  esperarVehiculo(mercedes, 2);
  esperarVehiculo(chevrolet, 3);
  
  nPrintf("--- Test: se ocupan todos los estacionamientos, "
          "ultimo espera ---\n");
  iniciar();
  renault= estacionarVehiculo("renault", 1, 2, 3);
  pausa(1);
  mercedes= estacionarVehiculo("mercedes", 1, 3, 2);
  pausa(1);
  chevrolet= estacionarVehiculo("chevrolet", 1, 6, 1);
  esperarVehiculo(renault, 1);
  esperarVehiculo(mercedes, 2);
  esperarVehiculo(chevrolet, 4);

  nPrintf("--- Test: estacionamientos se otorgan por orden de llegada ----\n");
  iniciar();
  renault= estacionarVehiculo("renault", 1, 2, 4);
  pausa(1);
  mercedes= estacionarVehiculo("mercedes", 1, 3, 3);
  pausa(1);
  chevrolet= estacionarVehiculo("chevrolet", 1, 6, 1);
  pausa(1);
  suzuki= estacionarVehiculo("suzuki", 1, 1, 1);
  esperarVehiculo(renault, 1);
  esperarVehiculo(mercedes, 2);
  esperarVehiculo(chevrolet, 5);
  esperarVehiculo(suzuki, 5);

  nPrintf("--- Test: un test mas completo ----\n");
  iniciar();
  chevrolet= estacionarVehiculo("chevrolet", 1, 6, 5);
  pausa(1);
  toyota= estacionarVehiculo("toyota", 1, 4, 3);
  pausa(1);
  renault= estacionarVehiculo("renault", 1, 2, 4);
  pausa(1);
  mg= estacionarVehiculo("mg", 1, 2, 3);
  pausa(1);
  suzuki= estacionarVehiculo("suzuki", 1, 1, 5);
  pausa(1);
  ford= estacionarVehiculo("ford", 1, 6, 2);
  pausa(1);
  mercedes= estacionarVehiculo("mercedes", 1, 3, 3);
  pausa(1);
  bmw= estacionarVehiculo("bmw", 1, 7, 1);
  pausa(1);
  esperarVehiculo(renault, 5);
  esperarVehiculo(mercedes, 9);
  esperarVehiculo(chevrolet, 1);
  esperarVehiculo(suzuki, 6);
  esperarVehiculo(toyota, 2);
  esperarVehiculo(mg, 5);
  esperarVehiculo(ford, 9);
  esperarVehiculo(bmw, 11);
}

//===================================================================
// Test de robustez
//

#ifdef NSYSTEM
#define NTASKS 100
#define ITER 30000
#else
#define NTASKS 100
#define ITER 500
#endif

void testRobustez() {
  nSetTimeSlice(1); // No hace nada en pSystem
  Vehiculo *vehiculos[NTASKS];
  char *noms[NTASKS];
  verbose= FALSE;
  nPrintf("Test de robustez -------------------------------------\n");
  for (int i= 0; i<NTASKS; i++) {
    noms[i]= nMalloc(10);
    sprintf(noms[i], "T%d", i);
    int k= (nRandom() % N_EST)+1;
    vehiculos[i]= estacionarVehiculo(noms[i], ITER, k, 0);
  }
  for (int i= 0; i<NTASKS; i++) {
    esperarVehiculo(vehiculos[i], -1);
    nFree(noms[i]);
  }
}

int nMain() {
#ifdef INIT
  initReservar();
#endif
  m= nMakeMonitor();
  t_mon= nMakeMonitor();
  testUnitarios();
  testRobustez();

  // endReservar();
  nPrintf("Felicitaciones: aprobo todos los tests\n");
  return 0;
}
