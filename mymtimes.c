#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_TIME_INTERVALS 24

void Modified_Files_Count(char *dir, int mcount[MAX_TIME_INTERVALS])
{
  DIR *d_handle;
  struct dirent *record;
  struct stat status_buffer;
  char path[2048];

  if((d_handle = opendir(dir)) == NULL)
  {
    printf("Cannot open directory: %s\n", dir);
    return;
  }

  chdir(dir);
  while((record = readdir(d_handle)) != NULL)
  {
    lstat(record->d_name, &status_buffer);
    if(S_ISDIR(status_buffer.st_mode))
    {
      if(strcmp(".", record->d_name) == 0 || strcmp("..", record->d_name) == 0)
        continue;
      Modified_Files_Count(record->d_name, mcount);
    }
    else
    {
      time_t current = time(NULL);
      int diff = (int)(current - status_buffer.st_mtime) / 3600;
      if(diff < MAX_TIME_INTERVALS)
        mcount[diff]++;
    }
  }
  chdir("..");
  closedir(d_handle);
}

int main(int argc, char *argv[])
{
  int i, mcount[MAX_TIME_INTERVALS] = {0};
  char *dir = ".";
  char timestr[50];
  time_t current_time = time(&current_time);

  if(argc == 2)
    dir = argv[1];

  Modified_Files_Count(dir, mcount);

  for (i = 0; i < MAX_TIME_INTERVALS; i++)
  {
        time_t hour_time = current_time - i * 3600;
        strftime(timestr, sizeof(timestr), "%a %b %d %H:%M:%S %Y", localtime(&hour_time));
        printf("%s: %d\n", timestr, mcount[i]);
  }

  return 0;
}
