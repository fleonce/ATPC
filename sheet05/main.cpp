#include "barrier.h"
#include <iostream>
#include <chrono>
#include <cassert>
#include "omp.h"

#define NUM_ITERATIONS 10
#define N 10000
#define M 1024
#define NUM_THREADS 128
#define BARRIER_TYPE 0

int is_prime(int p);

int main() {
    Barrier barrier = {};
    std::cout << "Barrier type = " << BARRIER_TYPE << std::endl;

    for(int n_threads = 2; n_threads <= NUM_THREADS; n_threads *= 2) {
        std::cout << "Num threads = " << n_threads << std::endl;
        double times[NUM_ITERATIONS]{0};
        for(int k = 0; k < NUM_ITERATIONS; k++) {
            int a = 3;
            int data[n_threads * M]{0};
            auto start_time = std::chrono::high_resolution_clock::now();
#pragma omp parallel shared(barrier, data) num_threads(n_threads)
            {
                int thread_num = omp_get_thread_num();
                for(int j = 0; j < M; j++) {
                    data[thread_num * M + j] = 1;
                }
                if(BARRIER_TYPE == 0) {
                    barrier.arrive();
                } else {
#pragma omp barrier
                }

                int sum = 0;
                for (int i = 0; i < n_threads * M; i++) {
                    sum += data[i];
                }
                if (sum != n_threads * M) {
                    // without barrier, print will occur
                    std::cout << sum << std::endl;
                }
            };
            auto end_time = std::chrono::high_resolution_clock::now();
            auto time = (end_time - start_time) / std::chrono::microseconds(1);
            times[k] = time;
        }

        double avg_time = 0;
        for (int k = 0; k < NUM_ITERATIONS; k++) {
            avg_time += times[k];
        }
        avg_time = avg_time / NUM_ITERATIONS;
        std::cout << "Time = " << avg_time << " ns" << std::endl;

    }

}

int is_prime(int p) {
    int d;
    for (d = 2; d < p; d = d + 1)
        if (p % d == 0)
            return 0;
    return 1;
}