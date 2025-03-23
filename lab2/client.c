#include <stdio.h>

#ifndef DYNAMIC
#include "collatz.h"
#else
#include <dlfcn.h>
#endif


int main() {
    #ifndef DYNAMIC
    char library_type[] = "non-dynamic library";
    #else
    char library_type[] = "dynamic library";

    void *handle = dlopen("./libcollatz.so", RTLD_LAZY);

    int (*test_collatz_convergence)();
    test_collatz_convergence = (int (*)())dlsym(handle, "test_collatz_convergence");
    #endif

    printf("%s\n", library_type);

    int numbers[5] = {11, 5, 27, 3, 8};
    int max_iter = 100;
    for (int i = 0; i < 5; i++) {
        int steps[max_iter];
        int iter_count = test_collatz_convergence(numbers[i], 100, steps);


        if (iter_count != 0) {
            printf("Number %d converged after %d steps with sequence: ", numbers[i], iter_count);
            for (int j = 0; j <= iter_count; j++) {
                printf("%d, ",steps[j]);
            }
            printf("\n");
        } else {
            if (numbers[i] == 1) {
                printf("Number is already 1\n");
            } else {
                printf("Number %d not converged within %d steps", numbers[i], max_iter);
                printf("\n");
            }
        }
    }
    return 0;
}