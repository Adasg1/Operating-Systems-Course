#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

#define QUEUE_SIZE 10
#define TEXT_SIZE 10
#define MAX_PRINTERS 3

typedef struct {
    char texts[QUEUE_SIZE][TEXT_SIZE + 1];
    int front;
    int rear;
    int count;
} PrintQueue;

typedef struct {
    PrintQueue queue;
    int printer_status[MAX_PRINTERS];  // 0-dostepna 1-wolna
    int taken_ids[MAX_PRINTERS];  // 0-dostepne 1-zajete
} SharedMemory;

SharedMemory *shared_memory;
sem_t *mutex;
int printer_id;

void handle_signal(int sig) {
    sem_wait(mutex);
    shared_memory->taken_ids[printer_id] = 0;  // zwalniamy ID drukarki
    sem_post(mutex);
    printf("Drukarka %d odlaczona", printer_id);
    exit(0);
}

int main() {
    const char *shm_name = "/print_system_shm";
    const char *queue_sem_name = "/queue_sem";
    const char *print_sem_name = "/print_sem";
    const char *mutex_name = "/mutex";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);  // otwieramy pamiec wspoldzielona
    shared_memory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *queue_sem = sem_open(queue_sem_name, 0); // otwieramy semafory
    sem_t *print_sem = sem_open(print_sem_name, 0);
    mutex = sem_open(mutex_name, 0);

    sem_wait(mutex);  // blokada dostępu do pamięci współdzielonej

    printer_id = -1;
    for (int i=0; i<MAX_PRINTERS; i++) {
        if (shared_memory->taken_ids[i] == 0) {
            printer_id = i;
            shared_memory->taken_ids[i] = 1;
            break;
        }
    }

    if (printer_id == -1) {
        printf("Nie ma dostepnych drukarek\n");
        sem_post(mutex);
        exit(1);
    }

    sem_post(mutex);  // odblokuj pamięć

    printf("Drukarka %d wlączona\n", printer_id);

    signal(SIGINT, handle_signal);

    while (1) {
        sem_wait(print_sem); // czekamy na zlecenie drukowania
        sem_wait(mutex);

        char text[TEXT_SIZE + 1];
        strcpy(text, shared_memory->queue.texts[shared_memory->queue.front]);
        shared_memory->queue.front = (shared_memory->queue.front + 1) % QUEUE_SIZE;
        shared_memory->queue.count--;

        shared_memory->printer_status[printer_id] = 1; // drukarka zajeta

        sem_post(mutex);
        sem_post(queue_sem);

        printf("Drukarka %d: ", printer_id); // drukowanie
        for (int i = 0; i < TEXT_SIZE; i++) {
            putchar(text[i]);
            fflush(stdout);
            sleep(1);
        }
        printf("\n");

        sem_wait(mutex);  // zwalniamy drukarke
        shared_memory->printer_status[printer_id] = 0;
        sem_post(mutex);
    }

    return 0;
}