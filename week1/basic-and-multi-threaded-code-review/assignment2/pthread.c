#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

#define COUNT 1000

typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[2];           // Array thread identiers
threadParams_t threadParams[2]; //Array of parameters to thread function

// Unsafe global
int gsum = 0;

void *incThread(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for (int i = 0; i < COUNT; i++) // Will do COUNT number of increments of gsum
    {
        gsum = gsum + i;
        printf("Increment thread idx=%d, gsum=%d\n", threadParams->threadIdx, gsum);
    }
}

void *decThread(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for (int i = 0; i < COUNT; i++) // Will do COUNT number of decrements of gsum
    {
        gsum = gsum - i;
        printf("Decrement thread idx=%d, gsum=%d\n", threadParams->threadIdx, gsum);
    }
}

int main(int argc, char *argv[])
{
    int i = 0;

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
}
