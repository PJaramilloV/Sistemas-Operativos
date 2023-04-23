// A6P1 fue en papel

//Scheduler con prioridades

static NthQueue *nth_pri1ReadyQueue;

// Pasar thread a estado READY
static void nth_pri1SetReady(nThread th){
    CHECK_CRITICAL("nth_fcfsSetReady")
    if(th-> status == READY || th->status == RUN)
        nFatalError("nth_fcfsReady", "Already in READY status\n");
    th->status = READY;
    nth_putBack(nth_pri1ReadyQueue[th->pri], th);
}

// Pasar thread a estado WAIT
static void nth_pri1Suspend(State waitState){
    CHECK_CRITICAL("nth_fcfsSuspend")
    nThread th = nSelf();
    if(th-> status != RUN || th->status != READY)
        nFatalError("nth_fcfsSuspend", "Thread not ready or running\n");
    th->status = waitState;
}

// Scheduler
static void nth_pri1Schedule(void ){
    CHECK_CRITICAL("nth_fcfsSchedule")

    // int prevCoreId= coreId();
    if(thisTh != NULL && (thisTh->status == READY || thisTh->status == RUN)){
        thisTh->status = READY;
        nth_putBack(nth_pri1ReadyQueue[thisTh->pri], thisTh);
    }

    nThread nextTh = NULL;
    while(nextTh == NULL){
        int i = 0;
        while( i< MAXPRI){
            // there is an element with priority "i"
            if(! nth_emptyQueue(nth_pri1ReadyQueue[i]))
                break;
            i++;
        }
        if(i<MAXPRI){
            nextTh= nth_getFront(nth_pri1ReadyQueue[i])
            break;
        } else {
            // No thread to execute, only a signal can wake up a thread, 
            // but those are disabled due to the critical section,
            // thus they must be enabled while waiting
            nth_coreIsIdle = 1; // to prevent recursivity
            sigset_t sigsetOld;
            pthread_sigmask(SIG_UNBLOCK, &nth_sigsetCritical, &sigset_Old); // reactivate signals
            llWait(&nth_nthreadReadyCond, &nth_schedMutex); // wait

            pthread_sigmask(SIG_SETMASK, &sigset_Old, NULL); // deactivate signals to recover critical section
            nth_coreIsIdle = 0;
        }

        nth_changeContext(thisTh, nextTh);

        nth_setSelf(thisTh);
        thisTh->status = ;
    }
}