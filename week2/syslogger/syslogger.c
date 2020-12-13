/// Initialize logging
#include <stdio.h>
#include "syslogger.h"

// Buffer to capture result of shell command
#define STREAM_BUFFER_SIZE 100

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