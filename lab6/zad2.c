#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/integral_fifo"

int main() {
    double a, b;
    printf("Podaj przedzial calkowania (a b)\n");
    scanf("%lf %lf", &a, &b);

    mkfifo(FIFO_PATH, 0666);

    if (fork() == 0) {
        execl("./calculate", "calculate", NULL);
        perror("execl");
        exit(1);
    }

    int fd = open(FIFO_PATH, O_WRONLY);
    write(fd, &a, sizeof(double));
    write(fd, &b, sizeof(double));
    close(fd);

    fd = open(FIFO_PATH, O_RDONLY);
    double result;
    read(fd, &result, sizeof(double));
    close(fd);

    printf("Wynik: %lf\n", result);

    unlink(FIFO_PATH);
    return 0;
}