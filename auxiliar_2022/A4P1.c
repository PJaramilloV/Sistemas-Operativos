// Semaforos

// herramienta de sync que consiste en un sistema de tickets,
// si hay tickets disponibles puedes acceder a la seccion critica,
// si no hay, debes esperar



/* Baño compartido entre fans de equipos de futbol
 * tamaño ilimitado pero fans del mismo equipo no
 * pueden estar al mismo tiempo
 * 
 * equipos: ROJO y AZUL
 **/
#define TRUE 1
#define FALSE 0

// ver que esto no funciona

enum {ROJO = 0, AZUL = 1};
int mutex = FALSE;
int cantidad[2] = {0,0};

void entrar(int color){
    if(cantidad[color] == 0){
        while(mutex)
            ;
        mutex = TRUE;
    }
    cantidad[color]++;
}

void salir(int color){
    cantidad[color]--;
    if(cantidad[color] == 0)
        mutex = 0;
}

/* Basta con ver en un diagram de threads q rojo puede entrar 
 * mientras hay azules
 **/

