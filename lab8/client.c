#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

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
    int taken_ids[MAX_PRINTERS];  // 0-dostepne 1-wolne
} SharedMemory;


int main() {
    const char *shm_name = "/print_system_shm";
    const char *queue_sem_name = "/queue_sem";
    const char *print_sem_name = "/print_sem";
    const char *mutex_name = "/mutex";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666); // otwieramy pamiec wspoldzielona
    SharedMemory *shared_memory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t *queue_sem = sem_open(queue_sem_name, 0); // otwieramy semafory
    sem_t *print_sem = sem_open(print_sem_name, 0);
    sem_t *mutex = sem_open(mutex_name, 0);

    srand(time(NULL) ^ getpid());

    while (1) {
        char text[TEXT_SIZE + 1]; // losowy tekst
        for (int i = 0; i < TEXT_SIZE; i++) {
            text[i] = 'a' + rand() % 26;
        }
        text[TEXT_SIZE] = '\0';

        printf("Klient %d: chce wydrukować: %s\n", getpid(), text);

        sem_wait(queue_sem); //czekamy na miejsce w kolejce
        sem_wait(mutex);

        strcpy(shared_memory->queue.texts[shared_memory->queue.rear], text); //dodajemy do kolejki
        shared_memory->queue.rear = (shared_memory->queue.rear + 1) % QUEUE_SIZE;
        shared_memory->queue.count++;

        printf("Klient %d: dodano zamówienie do kolejki. Wielkość kolejki: %d\n", getpid(), shared_memory->queue.count);

        sem_post(mutex);
        sem_post(print_sem);

        int sleep_time = 1 + rand() % 10; // klient czeka (1-10 sekund)
        sleep(sleep_time);
    }

    return 0;
}