#include <iostream>
#include <chrono>
#include <thread>
#include "mandsqueue.h"
#include "omp.h"

#define CONSENSUS_T 1
#define NUM_THREADS 2
#define NUM_RUNS 10000

int main_atomic_int() {
    std::cout << "Consensus = atomic_int" << CONSENSUS_T << std::endl;
    int val = 0;
    for (int i = 0; i < NUM_RUNS; i++) {
        std::atomic_int queue{0};
#pragma omp parallel num_threads(NUM_THREADS) shared(val)
        {
            int prev = queue.fetch_add(1);
#pragma omp barrier
            if (prev == 0) {
                val += 1;
            }
#pragma omp barrier
        };
    }

    std::cout << "Val = " << val << std::endl;
    return 0;
}

int main_queue() {
    std::cout << "Consensus = m&s_queue" << CONSENSUS_T << std::endl;
    int val = 0;
    for (int i = 0; i < NUM_RUNS; i++) {
        ms_queue<int> queue{0};
#pragma omp parallel num_threads(NUM_THREADS) shared(val)
        {
            queue.enqueue(omp_get_thread_num());
#pragma omp barrier
            int pos = queue.dequeue();
            if (pos == 0) {
                val += 1;
            }
#pragma omp barrier
        };
    }

    std::cout << "Val = " << val << std::endl;
    return 0;
}