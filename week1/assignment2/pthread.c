#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <syslog.h>

#define NUM_THREADS 128
#define STREAM_BUFFER_SIZE 1000

typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

void *threadRunner(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp;
    int sum = 0, threadNum;
    threadNum = threadParams->threadIdx + 1;

    for (int i = 1; i <= threadNum; i++)
        sum += i;
    char logbuffer[42] = {0};
    snprintf(logbuffer, 41, "Thread idx=%d, sum[1...%d]=%d\n",
             threadNum,
             threadNum, sum);
    syslog(LOG_INFO, logbuffer);
    // Thread idx=10, sum[1...128]=8256
}

int main(int argc, char *argv[])
{
    const char ident[] = "[COURSE:1][ASSIGNMENT:2]";
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

    for (int i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;
        pthread_create(&threads[i], (void *)0, threadRunner, (void *)&(threadParams[i]));
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("TEST COMPLETE\n");
    closelog();
}
