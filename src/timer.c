#include "server.h"

void timer(struct targ time_args){

    struct timeval decrement;
    decrement.tv_sec = time_args.interval;
    decrement.tv_usec = 0;

    while(t->tv_sec > 0){
        sleep(time_args.interval);
        timersub(time_args.t, &decrement, time_args.t);
        message_clients(update_time((int)decrement.tv_sec));
    }
    
}

