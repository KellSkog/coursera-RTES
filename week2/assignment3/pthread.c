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
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>

#define COUNT 100
#define STREAM_BUFFER_SIZE 100

typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[2];
threadParams_t threadParams[2];

// Unsafe global
int gsum = 0;

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

void *incThread(void *threadp)
{
    int i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for (i = 0; i < COUNT; i++)
    {
        gsum = gsum + i;
        syslog(LOG_INFO, "Increment thread idx=%d, gsum=%d\n", threadParams->threadIdx, gsum);
    }
}

void *decThread(void *threadp)
{
    int i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for (i = 0; i < COUNT; i++)
    {
        gsum = gsum - i;
        syslog(LOG_INFO, "Decrement thread idx=%d, gsum=%d\n", threadParams->threadIdx, gsum);
    }
}

int main(int argc, char *argv[])
{
    int rc;
    int i = 0;
    if (init_logging() != 0)
    {
        //Log initialization failed
        return -1;
    }
    threadParams[i].threadIdx = i;
    pthread_create(&threads[i],               // pointer to thread descriptor
                   (void *)0,                 // use default attributes
                   incThread,                 // thread function entry point
                   (void *)&(threadParams[i]) // parameters to pass in
    );
    i++;

    threadParams[i].threadIdx = i;
    pthread_create(&threads[i], (void *)0, decThread, (void *)&(threadParams[i]));

    for (i = 0; i < 2; i++)
        pthread_join(threads[i], NULL);

    printf("TEST COMPLETE\n");
    closelog();
}
