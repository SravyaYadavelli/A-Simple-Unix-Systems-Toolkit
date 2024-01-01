#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int c_status;
  struct rusage c_usage;
  struct timeval stime, etime;

  if (argc < 2)
  {
    printf("Usage: %s cmd [arguments]\n", argv[0]);
    return 1;
  }

  int cpid = fork();
  if (cpid == -1)
  {
    printf("Error in creating child process \n");
    return 1;
  }

  gettimeofday(&stime,NULL);

  if (cpid == 0)
  {
    execvp(argv[1], &argv[1]);
    printf("Error in execvp");
    return 1;
  }

  if (wait4(cpid, &c_status, 0, &c_usage) == -1)
  {
    printf("Error with wait4 call");
    return 1;
  }

  gettimeofday(&etime,NULL);

  long user_time_sec = c_usage.ru_utime.tv_sec;
  long user_time_usec = c_usage.ru_utime.tv_usec;

  long system_time_sec = c_usage.ru_stime.tv_sec;
  long system_time_usec = c_usage.ru_stime.tv_usec;

  long seconds= etime.tv_sec - stime.tv_sec;
  long micro_seconds= ((seconds*1000000)+etime.tv_usec)-(stime.tv_usec);

  printf("User CPU time: %ld.%06ld sec\n", user_time_sec, user_time_usec);
  printf("System CPU time: %ld.%06ld sec\n", system_time_sec, system_time_usec);
  printf("Elapsed time: %ld.%06ld sec\n", seconds, micro_seconds);
  return 0;
}
