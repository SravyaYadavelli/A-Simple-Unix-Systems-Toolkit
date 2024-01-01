#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>

struct record{
  char *data;
  int is_directory;
  struct record *next;
};

void flow(const char* pwd, const char* front)
{
  struct record *head = NULL;
  struct record *cnode, *tail;
  struct dirent *fd;
  int i,size = 0;
  DIR *dh;
  char *path, *alignment, *add_on, *next_front;

  dh = opendir(pwd);

  if (!dh) {
    fprintf(stderr, "Cannot open the directory \"%s\"\n", pwd);
    return;
  }

  while ((fd = readdir(dh)) != NULL)
  {
    if (fd->d_name[0] == '.')
    {
      continue;
    }
    cnode = malloc(sizeof(struct record));
    cnode->data =strcpy(malloc(strlen(fd->d_name) + 1),fd->d_name);
    cnode->is_directory = fd->d_type == DT_DIR;
    cnode->next = NULL;
    if (head == NULL)
    {
      head = cnode;
      tail =cnode;
    }
    else
    {
      tail->next = cnode;
      tail = cnode;
    }
    size++;
  }

  closedir(dh);

  if (!head)
  {
    return;
  }

  for ( i = 0; i < size; i++)
  {
    if (i == size - 1)
    {
      alignment = "|---";
      add_on = "    ";
    }
    else
    {
      alignment = "|---";
      add_on = "|   ";
    }
    printf("%s%s%s\n", front, alignment, head->data);
    if (head->is_directory)
    {
      path = malloc(strlen(pwd) + strlen(head->data) + 2);
      sprintf(path, "%s/%s", pwd, head->data);

      next_front = malloc(strlen(front) + strlen(add_on) + 1);
      sprintf(next_front, "%s%s", front, add_on);

      flow(path, next_front);

      free(path);
      free(next_front);
    }
    cnode = head;
    head = head->next;

    free(cnode->data);
    free(cnode);
  }
}

void main(int argc, char *argv[])
{
  char* pwd;
  if (argc > 1)
    pwd = argv[1];
  else
    pwd = ".";
  printf("%s\n", pwd);
  flow(pwd, "");
}
