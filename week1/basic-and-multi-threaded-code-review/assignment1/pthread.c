
/*
Threaded Hello world code walk-through
upload your code and a video demonstrating main functionalities of the code corresponding to the assignment 
combined in a zip file.

Start recording with the "mic enabled"  and then use tabs to show various interactive displays like 
"htop", "iostat", "tail -f /var/log/syslog", etc. and another tab for your code, 
showing that it builds and runs, and then walk through your code line by line and function by function explaining it.

The goal is to provide a build, demonstration, and source overview for your peer reviewer, 
explaining as best you can, with good commenting and clean code.
*/
#include <pthread.h>
#include <stdio.h>
#include <syslog.h>

#define NUM_THREADS 12
#define STREAM_BUFFER_SIZE 100 // Size of buffer used for syslog

typedef struct
{
    int threadIdx;
} threadParams_t;

// POSIX thread declarations and scheduling attributes
//
pthread_t threads[NUM_THREADS];           // Array of thread descritors
threadParams_t threadParams[NUM_THREADS]; // Array of parameters for thread function

// Function that executes in thread
// @param void pointer
void *counterThread(void *threadp)
{
    threadParams_t *threadParams = (threadParams_t *)threadp; // Give void parameter correct type
    if (threadParams->threadIdx == 0)                         // Only log first thread
        syslog(LOG_INFO, "Hello World from Thread!");
}

int main(int argc, char *argv[])
{
    const char ident[] = "[COURSE:1][ASSIGNMENT:1]";
    int option = 0, facility = LOG_USER;

    openlog(ident, 0, LOG_USER);           // Open syslog stream that puts 'ident' string at start of each line
    FILE *stream = popen("uname -a", "r"); // Open a stream to read result of shell command
    if (stream == NULL)
    {
        printf("Unable to send command to the shell\n");
        closelog();
        return -1;
    }
    char result[STREAM_BUFFER_SIZE] = {0x0};
    while (fgets(result, STREAM_BUFFER_SIZE, stream) != NULL)
        ; //reads the result from the command and stores it in the variable result
    pclose(stream);

    syslog(LOG_INFO, result);
    syslog(LOG_INFO, "Hello World from Main!");

    for (int i = 0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx = i;

        pthread_create(&threads[i],               // pointer to thread descriptor
                       (void *)0,                 // use default attributes
                       counterThread,             // thread function entry point
                       (void *)&(threadParams[i]) // parameters to pass in
        );
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    printf("The End\n");
    closelog();
}
