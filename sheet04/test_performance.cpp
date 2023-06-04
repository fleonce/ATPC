# include <chrono>
# include <thread>
#include <iostream>
#include <typeinfo>
# include "lock.h"

/* Configurations */
# define NUM_OF_THREADS 100
# define NUM_OF_ITERS 1000
# define OUTSIDE_WORK 10
# define NUM_OF_RUNS 3
# define LOCK_T MCSLock
# define N 124913


/* Private variables */
// thread_local int dummy ;
/* Child thread 's code */

int is_prime(int p) {
    int d;
    for (d = 2; d < p; d = d + 1)
        if (p % d == 0)
            return 0;
    return 1;
}

int main() {
    std::cout << "Num threads = " << NUM_OF_THREADS << std::endl;
    std::cout << "Num iters = " << NUM_OF_ITERS << std::endl;
    std::cout << "Outside work = " << OUTSIDE_WORK << std::endl;
    std::cout << "Lock type = " << typeid(LOCK_T).name() << std::endl;

    for (int k = 0; k < NUM_OF_RUNS; k++) {
        LOCK_T lock = LOCK_T();

        auto start_time = std::chrono::high_resolution_clock::now();
        // shared(lock)
#pragma omp parallel num_threads(NUM_OF_THREADS)
        {
            for (auto i = 0; i < NUM_OF_ITERS; ++i) {
                lock.acquire();
                if (is_prime(N + i)) {}
                lock.release();
                for (auto j = 0; j < OUTSIDE_WORK; ++j) {
                    if(is_prime(N)) {};
                }
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time = end_time - start_time;
        std::cout << time / std::chrono::milliseconds(1) << std::endl;
    }
}