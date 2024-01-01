#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_INPUT_LEN 80
#define MAX_ARGS 16
#define MAX_PIPES 5
#define LENGTH 100

void print_prompt();
void parse_input(char *input, char **args, int *argc);
void handle_pipe(char **args, int argc);
void handle_redirection(char **args, int *argc);
void myexit();
void mycd(char **args);
void mypwd();
void handle_command(char **args, int input_fd, int output_fd);

int main()
{
    char input[MAX_INPUT_LEN];
    char *args[MAX_ARGS];
    int argc,k;

    while (1)
    {
        k=0;
        print_prompt();
        if(gets(input)==NULL)
        {
                myexit();
        }
        if (strchr(input, '<') != NULL || strchr(input,'>') != NULL)
        {
                k=1;
        }
        if (strchr(input, '|') != NULL)
        {
                k=2;
        }
        parse_input(input, args, &argc);
        if (argc == 0)
        {
            continue;
        }
        if (k==1)
        {
                handle_redirection(args, &argc);
        }
        else if(k==2)
        {
                handle_pipe(args,argc);
        }
        else if (strcmp(args[0], "myexit") == 0)
        {
            myexit();
        }
        else if (strcmp(args[0], "mycd") == 0)
        {
            mycd(args);
        }
        else if (strcmp(args[0], "mypwd") == 0)
        {
            mypwd();
        }
        else
        {
            handle_command(args, STDIN_FILENO, STDOUT_FILENO);
        }
    }

    return 0;
}

void print_prompt() {
    printf("$ ");
}

void parse_input(char *input, char **args, int *argc)
{
    char *token;

    token = strtok(input, " ");
    *argc = 0;

    while (token != NULL) {
        args[*argc] = token;
        (*argc)++;
        token = strtok(NULL, " ");
    }

    args[*argc] = NULL;
}

void handle_pipe(char **args, int argc)
{
    int pipes[MAX_PIPES][2];
    int num_pipes = 0;
    int num_args;
    pid_t pid;
    int status,i,j;

    num_pipes = 0;
    for (i = 0; i < argc; i++)
    {
        if (strcmp(args[i], "|") == 0)
        {
            num_pipes++;
        }
    }
    if (num_pipes > MAX_PIPES)
    {
        printf("Too many pipes (max %d)\n", MAX_PIPES);
        return;
    }
    for (i=0; i<num_pipes;i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
    int arg_start = 0;
    for (i = 0; i <= num_pipes; i++)
    {
        num_args = 0;
        for (j = arg_start; j < argc; j++)
        {
            if (strcmp(args[j], "|") == 0)
            {
                break;
            }
            args[num_args] = args[j];
            num_args++;
        }
        args[num_args] = NULL;
        arg_start += num_args + 1;

        pid = fork();
        if (pid == 0)
        {
            if (i > 0)
            {
                if (dup2(pipes[i-1][0], STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(pipes[i-1][0]);
            }
            if (i < num_pipes)
            {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(pipes[i][1]);
            }

            for(j = 0; j < num_pipes; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else
        {
            if (i > 0)
            {
                close(pipes[i-1][0]);
                close(pipes[i-1][1]);
            }
        }
    }

    for(i=0;i<num_pipes;i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for(i=0;i<num_pipes+1;i++)
    {
        wait(&status);
    }
}


void handle_redirection(char **args, int *argc)
{
    int i = 0;
        int input_fd, output_fd;

   input_fd= STDIN_FILENO;
   output_fd= STDOUT_FILENO;
    while (args[i] != NULL)
        {
        if (strcmp(args[i], "<") == 0)
                {
            if (args[i+1] == NULL)
                        {
                fprintf(stderr, "syntax error near unexpected token '<'\n");
                exit(1);
            }
            input_fd = open(args[i+1], O_RDONLY);
            if (input_fd < 0)
                        {
                perror("open");
                exit(1);
            }
            memmove(&args[i], &args[i+2], (*argc-i-1) * sizeof(char *));
            *argc -= 2;
                        break;
        }
        i++;
    }

    i = 0;
    while (args[i] != NULL)
        {
        if (strcmp(args[i], ">") == 0)
                {
            if (args[i+1] == NULL)
                        {
                fprintf(stderr, "syntax error near unexpected token '>'\n");
                exit(1);
            }
            output_fd = open(args[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd < 0)
                        {
                perror("open");
                exit(1);
            }
            memmove(&args[i], &args[i+2], (*argc-i-1) * sizeof(char *));
            *argc -= 2;
                        break;
        }
        i++;
    }
        handle_command(args, input_fd, output_fd);
}

void myexit()
{
    printf("Exiting mytoolkit...\n");
    exit(0);
}

void mycd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "mycd: expected argument to \"mycd\"\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("mycd");
        }
    }
}

void mypwd()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("%s\n", cwd);
    }
    else
    {
        perror("mypwd");
    }
}

void handle_command(char **args, int input_fd, int output_fd)
{
    int pid, status;

    pid = fork();

    if (pid == 0)
        {

        if (input_fd != STDIN_FILENO)
                {
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_fd != STDOUT_FILENO)
                {
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        if (execvp(args[0], args) == -1)
                {
            perror("shell");
        }
        exit(EXIT_FAILURE);
    }
        else if (pid < 0)
        {
        perror("shell");
    }
        else
        {
        do
                {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}