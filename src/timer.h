struct targ {
    struct timeval *t;
    int interval;
};

void timer(struct timeval* t, int interval);
