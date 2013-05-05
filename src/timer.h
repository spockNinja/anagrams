struct targ {
    struct timeval *t;
    int interval;
};

void timer(void *t_args);
