#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

volatile sig_atomic_t mode = 0;
volatile sig_atomic_t request_count = 0;
volatile sig_atomic_t running = 1;
volatile sig_atomic_t counter = 0;

void handle_signal(int sig, siginfo_t *info, void *context) {
    if (sig == SIGUSR1) {
        mode = info->si_value.sival_int;
        request_count++;
        kill(info->si_pid, SIGUSR1);
    }
}

void handle_CtrlC(int sig) {
    printf("Wcisnieto CTRL+C\n");
    signal(SIGINT, handle_CtrlC);
}

int main(int argc, char **argv) {
    printf("Catcher's pid: %d\n", getpid());

    struct sigaction sa;

    sa.sa_sigaction = &handle_signal;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    while (running) {
        switch (mode) {
            case 1:
                signal(SIGINT, SIG_DFL);
                printf("Liczba otrzymanych rządań: %d\n",request_count);
                mode = 0;
                break;
            case 2:
                signal(SIGINT, SIG_DFL);
                printf("%d\n", counter++);
                sleep(1);
                break;
            case 3:
                signal(SIGINT, SIG_IGN);
                mode = 0;
                break;
            case 4:
                signal(SIGINT, handle_CtrlC);
                mode = 0;
                break;
            case 5:
                running = 0;
                mode = 0;
                break;
            default:
                pause();
                break;
        }
    }

    printf("Koniec działania catchera\n");
    return 0;
}