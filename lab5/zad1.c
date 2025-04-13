#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int sig_no) {
    printf("Received signal: %d.\n", sig_no);
}

int main(int argc,  char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Error - give one argument (none, ignore, handler, mask)\n");
        return 22;
    }

    if (strcmp("none", argv[1]) == 0) {
        printf("nie zmieniam reakcji na sygnal");
    }

    if (strcmp("ignore", argv[1]) == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    
    if (strcmp("handler", argv[1]) == 0) {
        signal(SIGUSR1, &handler);
    }

    if (strcmp("mask", argv[1]) == 0) {
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &newmask, NULL);
    }

    raise(SIGUSR1);

    if(strcmp("mask", argv[1]) == 0) {
        sigset_t pending_signals;
        sigpending(&pending_signals);
        if (sigismember(&pending_signals, SIGUSR1)) {
            printf("signal awaits\n");
        } else printf("no signals\n");
    }

    return 0;
}