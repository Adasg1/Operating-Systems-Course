#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <unistd.h>

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

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666); // inicjalizacja pamieci wspolnej
    ftruncate(shm_fd, sizeof(SharedMemory));
    SharedMemory *shared_memory = mmap(0, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    memset(shared_memory, 0, sizeof(SharedMemory));  // wypelniamy tablice w pamieci wspolnej
    for (int i = 0; i < MAX_PRINTERS; i++) {
        shared_memory->printer_status[i] = 0;
        shared_memory->taken_ids[i] = 0;
    }

    sem_t *queue_sem = sem_open(queue_sem_name, O_CREAT, 0666, QUEUE_SIZE);   // tworzymy semafory
    sem_t *print_sem = sem_open(print_sem_name, O_CREAT, 0666, 0);
    sem_t *mutex = sem_open(mutex_name, O_CREAT, 0666, 1);

    printf("Serwer wlączony\n");
    printf("Wcisnij Enter zeby wylaczyć serwer i zwolnić pamiec\n");
    getchar();

    sem_close(queue_sem); // czyscimy semafory i pamiec przy zamykaniu programu
    sem_close(print_sem);
    sem_close(mutex);
    sem_unlink(queue_sem_name);
    sem_unlink(print_sem_name);
    sem_unlink(mutex_name);

    munmap(shared_memory, sizeof(SharedMemory));
    shm_unlink(shm_name);

    return 0;
}