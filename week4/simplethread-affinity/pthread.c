#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include "syslog.h"

#define NUM_THREADS 128
#define NUM_CPUS 4

// Buffer to capture result of shell command
#define STREAM_BUFFER_SIZE 100

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

#define SCHED_POLICY SCHED_FIFO

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

void set_scheduler(void)
{
    int max_prio, scope, result_code, cpuidx;
    cpu_set_t cpuset;

    printf("INITIAL ");
    print_scheduler();

    pthread_attr_init(&fifo_sched_attr);
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_POLICY);
    CPU_ZERO(&cpuset);
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
    threadParams_t *threadParams = (threadParams_t *)threadp;

    int sum = 0;
    for (int i = 1; i <= (threadParams->threadIdx); i++)
        sum = sum + i;

    // gettimeofday(&stopTime, 0);
    // stop = ((stopTime.tv_sec * 1000000.0) + stopTime.tv_usec) / 1000000.0;

    syslog(LOG_INFO, "Thread idx=%d, sum[1...%d]=%d Running on Core : %d\n",
           threadParams->threadIdx,
           threadParams->threadIdx, sum, sched_getcpu());
}

void *starterThread(void *threadp)
{
    int i, result_code;

    printf("starter thread running on CPU=%d\n", sched_getcpu());

    for (i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i + 1;

        pthread_create(&threads[i],               // pointer to thread descriptor
                       &fifo_sched_attr,          // use FIFO RT max priority attributes
                       counterThread,             // thread function entry point
                       (void *)&(threadParams[i]) // parameters to pass in
        );
    }

    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);
}

int init_logging(void)
{
    openlog("[COURSE:1][ASSIGNMENT:4]", 0, LOG_USER);
    FILE *cmd = popen("uname -a", "r"); // Create a file stream capturing output from shell command
    if (cmd == NULL)
    {
        printf("Unable to send command to the shell\n");
        closelog();
        return -1;
    }
    char result[STREAM_BUFFER_SIZE] = {0x0}; //Initialize buffer with zeroes
    while (fgets(result, STREAM_BUFFER_SIZE, cmd) != NULL)
        ; //reads the result from the command and stores it in the variable result
    pclose(cmd);
    syslog(LOG_INFO, "%s\n", result);

    return 0;
}

int main(int argc, char *argv[])
{
    int result_code;
    int i, j;
    cpu_set_t cpuset;

    if (init_logging())
        return -1; // Stop if logging fails

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

        for (j = 0; j < CPU_SETSIZE; j++)
            if (CPU_ISSET(j, &cpuset))
                printf(" %d", j);

        printf("\n");
    }

    pthread_create(&startthread,     // pointer to thread descriptor
                   &fifo_sched_attr, // use FIFO RT max priority attributes
                   starterThread,    // thread function entry point
                   (void *)0         // parameters to pass in
    );

    pthread_join(startthread, NULL);

    printf("\nTEST COMPLETE\n");
}
