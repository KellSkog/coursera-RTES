/*
The goal of this peer review is to comment existing code examples, 
single step debug them (with nemiver or ddd), practice adding syslog tracing and 
then explain your updated version to your peers.
*/
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sched.h>
#include <unistd.h>
#include "syslogger.h"

// Defines
#define NUM_THREADS 64
#define NUM_CPUS 4
#define SCHED_POLICY SCHED_FIFO
#define MAX_ITERATIONS (1000000)

typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
pthread_t mainthread;
pthread_t startthread;
threadParams_t threadParams[NUM_THREADS];

pthread_attr_t fifo_sched_attr;
pthread_attr_t orig_sched_attr;
struct sched_param fifo_param;

void print_scheduler(void)
{
    int schedType = sched_getscheduler(getpid());

    switch (schedType)
    {
    case SCHED_FIFO:
        printf("Pthread policy is SCHED_FIFO\n");
        break;
    case SCHED_OTHER:
        printf("Pthread policy is SCHED_OTHER\n");
        break;
    case SCHED_RR:
        printf("Pthread policy is SCHED_RR\n");
        break;
    default:
        printf("Pthread policy is UNKNOWN\n");
    }
}

// Change scheduling policy, to start a thread with RT scheduler, the calling
// thread first needs to switch to a RT scheduling policy
void set_scheduler(void)
{
    int max_prio, result_code, cpuidx;
    cpu_set_t cpuset;

    printf("INITIAL ");
    print_scheduler();

    // Change scheduler
    pthread_attr_init(&fifo_sched_attr);
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);

    // Change sets of cores available for scheduler
    CPU_ZERO(&cpuset); // clear set of cores available for scheduler
    cpuidx = (3);
    CPU_SET(cpuidx, &cpuset);
    pthread_attr_setaffinity_np(&fifo_sched_attr, sizeof(cpu_set_t), &cpuset);

    max_prio = sched_get_priority_max(SCHED_POLICY);
    fifo_param.sched_priority = max_prio;

    if ((result_code = sched_setscheduler(getpid(), SCHED_POLICY, &fifo_param)) < 0)
        perror("sched_setscheduler");

    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);

    printf("ADJUSTED ");
    print_scheduler();
}

void *counterThread(void *threadp)
{
    int sum = 0, i, result_code, iterations;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    pthread_t mythread;
    double start = 0.0, stop = 0.0;
    struct timeval startTime, stopTime;

    gettimeofday(&startTime, 0);
    start = ((startTime.tv_sec * 1000000.0) + startTime.tv_usec) / 1000000.0;

    for (iterations = 0; iterations < MAX_ITERATIONS; iterations++)
    {
        sum = 0;
        for (i = 1; i < (threadParams->threadIdx) + 1; i++)
            sum = sum + i;
    }

    gettimeofday(&stopTime, 0);
    stop = ((stopTime.tv_sec * 1000000.0) + stopTime.tv_usec) / 1000000.0;

    syslog(LOG_INFO, "\nThread idx=%d, sum[0...%d]=%d, running on CPU=%d, start=%lf, stop=%lf",
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu(),
           start, stop);
}

void *starterThread(void *threadp)
{
    int i, result_code;

    syslog(LOG_INFO, "starter thread running on CPU=%d\n", sched_getcpu());

    for (i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;

        pthread_create(&threads[i],               // pointer to thread descriptor
                       &fifo_sched_attr,          // use FIFO RT max priority attributes
                       counterThread,             // thread function entry point
                       (void *)&(threadParams[i]) // parameters to pass in
        );
    }
    // Wait for all counter threads to complete
    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
}

int main(int argc, char *argv[])
{
    int result_code;
    cpu_set_t cpuset;

    set_scheduler();

    CPU_ZERO(&cpuset);

    // get affinity set for main thread
    mainthread = pthread_self();

    // Check the affinity mask assigned to the thread
    result_code = pthread_getaffinity_np(mainthread, sizeof(cpu_set_t), &cpuset);
    if (result_code != 0)
        perror("pthread_getaffinity_np");
    else
    {
        printf("main thread running on CPU=%d, CPUs =", sched_getcpu());

        for (int j = 0; j < CPU_SETSIZE; j++)
            if (CPU_ISSET(j, &cpuset))
                printf(" %d", j);

        printf("\n");
    }

    if (init_logging() != 0)
    {
        perror("Log initialization failed");
        exit(-1);
    }

    pthread_create(&startthread,     // pointer to thread descriptor
                   &fifo_sched_attr, // use FIFO RT max priority attributes
                   starterThread,    // thread function entry point
                   (void *)0         // parameters to pass in
    );

    pthread_join(startthread, NULL);

    printf("\nTEST COMPLETE\n");
}
