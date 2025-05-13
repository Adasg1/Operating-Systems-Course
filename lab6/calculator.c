#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>

#define FIFO_PATH "/tmp/integral_fifo"

double f(double x) {
    return 4/(x*x+1);
}

double calculate_integral_part(double start, double end, double dx) {
    double sum = 0.0;
    double x;
    for (x = start; x < end - dx; x += dx) {
        sum += f(x) * dx;
    }
    sum += f(x) * (end - x);
    return sum;
}

int main() {
    int fd = open(FIFO_PATH, O_RDONLY);
    double a;
    double b;
    read(fd, &a, sizeof(double));
    read(fd, &b, sizeof(double));
    close(fd);

    double result = calculate_integral_part(a, b, 0.0000001);

    fd = open(FIFO_PATH, O_WRONLY);
    write(fd, &result, sizeof(double));
    close(fd);
    
    return 0;
}
