#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>

#define NUM_THREADS 12
#define STREAM_BUFFER_SIZE 100
typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

void *counterThread(void *threadp)
{
    int sum = 0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp;
    if (threadParams->threadIdx == 0)
        syslog(LOG_INFO, "Hello World from Thread!"); // Only log first thread
    for (i = 1; i < (threadParams->threadIdx) + 1; i++)
        sum = sum + i;

    printf("Thread idx=%d, sum[0...%d]=%d\n",
           threadParams->threadIdx,
           threadParams->threadIdx, sum);
}

int main(int argc, char *argv[])
{
    int rc;
    int i;
    const char ident[] = "[COURSE:1][ASSIGNMENT:1]";
    int option = 0, facility = LOG_USER;

    openlog(ident, 0, LOG_USER);
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
    syslog(LOG_INFO, "Hello World from Main!");

    for (i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;

        pthread_create(&threads[i],               // pointer to thread descriptor
                       (void *)0,                 // use default attributes
                       counterThread,             // thread function entry point
                       (void *)&(threadParams[i]) // parameters to pass in
        );
    }

    for (i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("TEST COMPLETE\n");
    closelog();
}
