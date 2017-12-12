//quick hack for time measurement:
typedef unsigned long long usecs;

usecs mytime() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (usecs)now.tv_usec + (usecs)now.tv_sec*1000000L;
}


