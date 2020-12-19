#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include <sched.h>
#include <time.h>
#include <semaphore.h>

#include <syslog.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <errno.h>

#include <signal.h>

#define MY_CLOCK_TYPE CLOCK_MONOTONIC_RAW
#define NUM_THREADS (1)

struct timespec start_time_val;
double start_realtime;

static timer_t timer_1;
static struct itimerspec itime = {{1, 0}, {1, 0}};
static struct itimerspec last_itime;
volatile int wait = 500000000;
sem_t semS1;

void timer_isr()
{
    static int count = 0;
    printf("int_sec %ld, int_nano %ld, val_s %ld, val_nano %ld\n",
           itime.it_interval.tv_sec, itime.it_interval.tv_nsec,
           itime.it_value.tv_sec, itime.it_value.tv_nsec);
    if (++count > 10)
    {
        sem_post(&semS1);
        printf("Sem posted\n");
    }
}

int main()
{
    int result_code, rt_max_prio;
    pthread_attr_t rt_sched_attr[NUM_THREADS];
    struct sched_param main_param;
    pid_t mainpid = getpid();

    result_code = sched_getparam(mainpid, &main_param); //Retrieve default scheduling parameters (priority) for a particular process.
    rt_max_prio = sched_get_priority_max(SCHED_FIFO);
    // printf("Min = %d, Max = %d\n", sched_get_priority_min(SCHED_FIFO), sched_get_priority_max(SCHED_FIFO));
    main_param.sched_priority = rt_max_prio - 1;
    result_code = sched_setscheduler(getpid(), SCHED_FIFO, &main_param);
    if (result_code < 0)
        perror("main_param");
    // Main is now running SCHED_FIFO, if no error
    timer_create(CLOCK_REALTIME, NULL, &timer_1);
    signal(SIGALRM, (void (*)())timer_isr);
    itime.it_interval.tv_sec = 0; //Single expiration
    itime.it_interval.tv_nsec = 100000000;
    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 100000000;
    int flags = 0;
    if (sem_init(&semS1, 0, 0))
    {
        printf("Failed to initialize S1 semaphore\n");
        exit(-1);
    }
    timer_settime(timer_1, flags, &itime, &last_itime);

    // result_code = pthread_attr_init(&rt_sched_attr[i]);
    // result_code = pthread_attr_setinheritsched(&rt_sched_attr[i], PTHREAD_EXPLICIT_SCHED);
    // result_code = pthread_attr_setschedpolicy(&rt_sched_attr[i], SCHED_FIFO);
    // result_code = pthread_attr_setaffinity_np(&rt_sched_attr[i], sizeof(cpu_set_t), &threadcpu);

    // struct sched_param rt_param[NUM_THREADS];
    // rt_param[i].sched_priority = rt_max_prio - i;
    // pthread_attr_setschedparam(&rt_sched_attr[i], &rt_param[i]);

    // threadParams[i].threadIdx = i;

    // static struct timespec sleep_time = {4, 0};
    // static struct timespec remaining_time = {0, 0};
    // nanosleep(&sleep_time, &remaining_time);
    sem_wait(&semS1);
    itime.it_interval.tv_sec = 0; //Disarm
    itime.it_interval.tv_nsec = 0;
    itime.it_value.tv_sec = 0;
    itime.it_value.tv_nsec = 0;
    timer_settime(timer_1, flags, &itime, &last_itime);
    printf("Scheduler %d\n", sched_getscheduler(mainpid));
}