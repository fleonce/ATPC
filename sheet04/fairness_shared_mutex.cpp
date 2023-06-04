#include <omp.h>
#include <cassert>
#include <iostream>
#include <shared_mutex>
#include <atomic>
#include <thread>

#define MAX_ITER 20000
#define READ_THREADS 10

int main() {
    int n_threads = READ_THREADS;
    std::atomic_int counters[READ_THREADS] = {};
    std::shared_mutex mutex = std::shared_mutex();

#pragma omp parallel num_threads(n_threads)
    {
        //assert((omp_get_num_threads() == 2) && "OMP_NUM_THREADS != 2");

        int thread_num = omp_get_thread_num();
        for (int j = 0; j < MAX_ITER; j++) {
            counters[thread_num]++;
            if (thread_num != 0) {
                // read
                mutex.lock_shared();
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
                mutex.unlock_shared();
            } else {
                mutex.lock();
                std::this_thread::sleep_for(std::chrono::nanoseconds(10));
                mutex.unlock();
            }

            bool stop = false;
            for (int k = 0; k < n_threads; k++) {
                stop = counters[k].load() >= MAX_ITER;
                if (stop) break;
            }
            if (stop) break;
        }
#pragma omp barrier
    };


    for (int k = 0; k < n_threads; k++) {
        std::cout << "Thread " << k << ": " << counters[k].load() << std::endl;
    }
    return 0;
}