#include "barrier.h"
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>
#include "omp.h"


int Barrier::arrive() {
    int epoch = this->epoch.load();
    int n_threads = omp_get_num_threads();
    int prev_counter = this->ctr.fetch_add(1, std::memory_order_release);
    int done_threads = prev_counter + 1;
    bool is_last = done_threads == n_threads;
    if (is_last) {
        int active_epoch = this->epoch++;
        // std::cout << "reset from " << active_epoch << "" << std::endl;
        this->ctr.store(0, std::memory_order_relaxed);

        this->flag.store(active_epoch + 1, std::memory_order_release);
    } else {
        while(this->flag.load(std::memory_order_acquire) != (epoch + 1)) {}
    }

    // while(this->flag.load(std::memory_order_acquire) != (epoch + 1));
    return 0;
}
