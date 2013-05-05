#include "server.h"

void timer(struct timeval* t, int interval){

    struct timeval decrement;
    decrement.tv_sec = interval;
    decrement.tv_usec = 0;

    while(t.tv_sec > 0){
        sleep(interval);
        timersub(&t, &decrement, &t);
        message_clients(update_time((int)countdown.tv_sec));
    }
    
}

