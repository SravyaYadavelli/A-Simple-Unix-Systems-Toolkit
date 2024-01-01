#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <timeout> <command> [args ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int timeout = atoi(argv[1]);
    if (timeout <= 0) {
        fprintf(stderr, "Invalid timeout: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(argv[2], &argv[2]);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        sleep(timeout);
        if (kill(pid, SIGTERM) == -1) {
            perror("kill");
            exit(EXIT_FAILURE);
        }
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            exit(WEXITSTATUS(status));
        } else {
            exit(EXIT_FAILURE);
        }
    }
}