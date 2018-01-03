#include <sys/time.h>
typedef unsigned long long usecs;

double mytime() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return ((double)((usecs)now.tv_usec + (usecs)now.tv_sec*1000000.0))/1000000.0;
}


