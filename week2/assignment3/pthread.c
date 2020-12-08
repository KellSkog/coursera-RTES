/**
 * Boilerplate
 * This assignment requires to implement 128 real -time threads with FIFO scheduling policy printing the thread id [1 ...128] , 
 * sum from 1 to thread id and core which it is running on CPU. 
 * The assignment should be executed on Linux system running on raspberry pi. 
 * Submit the syslog file with the logs printed as below guidelines
 * 
Print the output of uname -a in the first line of the syslog file.
Each syslog statement should have [COURSE:X][ASSIGNMENT:Y] where X corresponds to Course Number i.e. 1 in this case and Y corresponds to Assignment No.
Its expected to have syslogs for 128 threads with respective thread idx and sum printed on a single line for respective thread in the following format as below
<Current System Time> <Host Name> [COURSE:1][ASSIGNMENT:3]: Thread idx=10, sum[1...10]=55 Running on core : XYZ

It's expected to use First Input First Output (FIFO) scheduling mechanism governing the execution of the threads with 99 as maximum priority and 1 as minimum priority.

Please find the started code attached for your reference
*/
#ifndef _GNU_SOURCE
#error "_GNU_SOURCE definition needed"
#endif

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#define STREAM_BUFFER_SIZE 100
#define NUM_THREADS 128 //thread id [1 ...128]

/// Initialize logging
int init_logging(void)
{
    openlog("[COURSE:1][ASSIGNMENT:3]", 0, LOG_USER);
    FILE *cmd = popen("uname -a", "r");
    if (cmd == NULL)
    {
        printf("Unable to send command to the shell\n");
        closelog();
        return -1;
    }
    char result[STREAM_BUFFER_SIZE] = {0x0};
    while (fgets(result, STREAM_BUFFER_SIZE, cmd) != NULL)
        ; //reads the result from the command and stores it in the variable result
    pclose(cmd);
    syslog(LOG_INFO, result);

    return 0;
}

typedef struct
{
    int threadIdx;
} threadParams_t;

void *runThread(void *threadp)
{
    int sum = 0;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    // int threadId = ((threadParams_t *)threadp)->threadIdx;
    int threadId = threadParams->threadIdx;
    for (int i = 1; i <= threadId; i++)
    {
        sum += i;
    }
    // Output example Thread idx=10, sum[1...10]=55 Running on core : XYZ
    syslog(LOG_INFO, "Thread idx=%d, sum[1...%d]=%d Running on core : %d\n",
           threadId, threadId, sum, sched_getcpu());
    if (threadId == 99)
    { // Verify scheduler for one thread
        int type = sched_getscheduler(getpid());
        if (type != SCHED_FIFO)
            printf("Wrong scheduler: %d\n", type);
    }
}

int main(int argc, char *argv[])
{
    // POSIX thread declarations and scheduling attributes
    pthread_t threads[NUM_THREADS];
    threadParams_t threadParams[NUM_THREADS];
    pthread_attr_t rt_sched_attr[NUM_THREADS];
    struct sched_param rt_param[NUM_THREADS];
    struct sched_param main_param;
    pthread_attr_t main_attr;

    if (init_logging() != 0)
    {
        //Log initialization failed
        return -1;
    }

    int rt_max_prio = sched_get_priority_max(SCHED_FIFO);
    main_param.sched_priority = rt_max_prio;
    if (sched_setscheduler(getpid(), SCHED_FIFO, &main_param) < 0)
        perror("******** WARNING: sched_setscheduler");

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_attr_init(&rt_sched_attr[i]);
        pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
        threadParams[i].threadIdx = i + 1;
        pthread_create(&threads[i],               // pointer to thread descriptor
                       (void *)0,                 // use default attributes
                       runThread,                 // thread function entry point
                       (void *)&(threadParams[i]) // parameters to pass in
        );
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("TEST COMPLETE\n");
    closelog();
}
