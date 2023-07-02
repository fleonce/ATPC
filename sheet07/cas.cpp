#include <atomic>
#include <iostream>
#include "omp.h"

int main() {
    std::atomic_int var;
    std::cout << var.load() << std::endl;
#pragma omp parallel num_threads(12)
    {
        int expect = 0;
        bool achievement;
        do {
            bool res = var.compare_exchange_strong(expect, -(omp_get_thread_num() + 1));
            if (res) {
                achievement = true;
                break;
            } else if (expect < 0) {
                achievement = false;
                break;
            }
        } while (true);

        if (achievement) {
            std::cout << "Achievement" << std::endl;
        }
    };
}