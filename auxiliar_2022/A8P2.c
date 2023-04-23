// Sincronizar 5 cores para que trabajen en conjunto

// team(): funcion que espera hasta que 5 cores la utilizen para coordinarse

int player(char *name){
    for(;;){
        char** mTeam = team(name);
        play(mTeam);
        sleep();
    }
}

// Programar team con spin locks usando malloc() y coreId()
enum {OPEN, CLOSED};

char** teamNames;
int cores = 0;
int* locks[4];
int mutex = OPEN;

char** team(char* name){
    spinLock(&mutex);
    if(cores == 0){
        teamNames = malloc(5*sizeof(char*));
    }


    teamNames[cores] = name;
    
    if(cores < 4){
        int lk = CLOSED;
        locks[cores] = &lk;
        cores++;
        spinLock(&locks[cores]);  // busy waiting
        
    } else {
        for(int i=0; i<4; i++){
            spinUnlock(locks[i]);
        }
        spinUnlock(&mutex);
    }
    return teamNames;
}