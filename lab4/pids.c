#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Error - give one argument\n");
        return 22;
    }

    int num_processes = atoi(argv[1]);

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return errno;
        }

        if (pid == 0) {
            printf("parent's pid: %d, current process's pid %d\n", getppid(), getpid());
            exit(0);
        }
    }

    for (int i = 0; i < num_processes; i++) { //macierzysty czeka na zakonczenie procesów potomnych
        wait(NULL);
    }

    printf("liczba procesów: %d\n", num_processes);
    return 0;
}
