#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

typedef struct {
    double dx;
    int thread_id;
    int num_threads;
    double* results;
} ThreadData;

double f(double x) {
    return 4.0 / (x * x + 1.0);
}

void* compute_integral(void* arg) {  // funkcja wykonywana dla każdego wątku
    ThreadData* data = (ThreadData*)arg;
    double dx = data->dx;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;

    double a = 0.0; // przedzial calkowania
    double b = 1.0;
    int n = (int)((b - a) / dx); // ilosc prostokątów
    
    double local_sum = 0.0;
    for (int i = thread_id; i < n; i += num_threads) {
        double x = a + i * dx;
        local_sum += f(x) * dx;
    }
    data->results[thread_id] = local_sum;
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Użycie: %s <szerokość_prostokąta> <max_wątków>\n", argv[0]);
        return 1;
    }
    
    double dx = atof(argv[1]);
    int max_threads = atoi(argv[2]);
    
    if (dx <= 0 || dx > 1 || max_threads <= 0) {
        fprintf(stderr, "Nieprawdlowe parametry\n");
        return 1;
    }
    
        for (int num_threads = 1; num_threads <= max_threads; num_threads++) {
            pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
            ThreadData* thread_data = malloc(num_threads * sizeof(ThreadData));
            double* results = calloc(num_threads, sizeof(double));
            
            struct timeval start, end;
            gettimeofday(&start, NULL);
            
            for (int i = 0; i < num_threads; i++) {
                thread_data[i].dx = dx;
                thread_data[i].thread_id = i;
                thread_data[i].num_threads = num_threads;
                thread_data[i].results = results;
                
                pthread_create(&threads[i], NULL, compute_integral, &thread_data[i]);
            }
            
            for (int i = 0; i < num_threads; i++) {
                pthread_join(threads[i], NULL);\
            }
            
            gettimeofday(&end, NULL);
            double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
            
            double total = 0.0; // sumujemy wyniki każego wątku
            for (int i = 0; i < num_threads; i++) {
                total += results[i];
            }
            
            printf("Liczba wątków: %2d | Wynik całki: %11.8f | Czas: %6f s\n", num_threads, total, time_spent);
            
            free(threads);
            free(thread_data);
            free(results);
        }
    return 0;
}