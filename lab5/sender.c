#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t confirmation_received;

void handle_confirmation(int sig) {
    confirmation_received = 1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Niepoprawna ilość argumentów");
        return 1;
    }

    pid_t catcher_pid = atoi(argv[1]);
    int mode = atoi(argv[2]);

    if (mode < 1 || mode > 5) {
        printf("Niepoprawny tryb (1-5)");
        return 1;
    }

    struct sigaction sa;
    sa.sa_handler = handle_confirmation;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    union sigval value;
    value.sival_int = mode;

    if (sigqueue(catcher_pid, SIGUSR1, value) == -1) {
        perror("sigqueue");
        return 1;
    }

    while (!confirmation_received) {
        pause();
    }

    printf("Sender otrzymał potwierdzenie dla trybu %d\n", mode);
    return 0;
}