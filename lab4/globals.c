#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

int global = 0;

int main(int argc, char *argv[]) {
    int local = 0;

    if (argc != 2) {
        fprintf(stderr, "error - no directory path given\n");
        return 22;
    }
    printf("program's name: %s\n", argv[0]);

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return errno;
    }

    if (pid == 0) {
        printf("\nchild process\n");
        local++;
        global++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n",local, global);
        int result = execl("/bin/ls", "ls", argv[1], (char *)NULL);
        if (result == -1) {
            perror("execl failed");
            return errno;
        }
        exit(0);
    }

    if (pid > 0) {
        int status;
        wait(&status);
        int exit_code = WEXITSTATUS(status);

        printf("\nparent process\n");
        printf("parent pid = %d, child pid = %d\n", getppid(), getpid());
        printf("child exit code: %d\n", exit_code);   // =0 oznacza poprawne wykonanie ls, =2 oznacza że ls zwrócił kod błędu
        printf("parent's local = %d, parent's global = %d\n", local, global);

    }

    return 0;
}