#include "collatz.h"

int collatz_conjecture(int input) {
    if (input % 2 == 0) {
        return input / 2;
    }
    return input * 3 + 1;
}

int test_collatz_convergence(int input, int max_iter, int *steps) {
    int current_number = input;
    for (int i = 0; i < max_iter; i++) {
        steps[i] = current_number;
        if (current_number == 1) {
            return i;
        }
        current_number = collatz_conjecture(current_number);
    }
    return 0;
}
