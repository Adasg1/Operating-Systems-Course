#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

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

void run_integration(int k, double dx) {
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    int pipes[k][2];
    pid_t pids[k];

    for (int i = 0; i < k; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork");
            exit(1);
        }

        if (pids[i] == 0) { // dziecko
            close(pipes[i][0]); // zamykamy odczyt bo dziecko będzie tylko zapisywać

            double start = i * (1.0 / k);
            double end = (i + 1) * (1.0 / k);
            double partial_sum = calculate_integral_part(start, end, dx);

            write(pipes[i][1], &partial_sum, sizeof(partial_sum));
            close(pipes[i][1]);
            exit(1);
        } else { //rodzic
            close(pipes[i][1]); // zamykamy zapis bo rodzic będzie tylko odczytywać
        }
    }

    double total = 0.0;
    for (int i = 0; i < k; i++) {
        double partial_sum;
        read(pipes[i][0], &partial_sum, sizeof(partial_sum));
        total += partial_sum;
        close(pipes[i][0]);
        waitpid(pids[i], NULL, 0);
    }

    gettimeofday(&end_time, NULL);
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) + 
                         (end_time.tv_usec - start_time.tv_usec) / 1e6;

    printf("k = %d, wynik = %.10f, czas = %.6f sekund\n", k, total, elapsed_time);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Podaj: %s (dlugosc przedzialu) (ilosc procesow)\n", argv[0]);
        return 21;
    }

    double dx = atof(argv[1]);
    int n = atoi(argv[2]);

    if (dx <= 0 || n <= 0) {
        fprintf(stderr, "przedzial dx i n musza byc dodatnie\n");
        return 21;
    }

    for (int k = 1; k <= n; k++) {
        run_integration(k, dx);
    }

    return 1;
}

// ./zad1 0.000000002 10 lub make test