#include "omp.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "sets.h"

#define N_THREADS 24
#define N_DATA 1000
// #define SET_T CoarseGrainedSet<int>


void performance_test(Set<int> &set) {
    // auto set = CoarseGrainedSet<int>();//SET_T();
    auto max_n_threads = N_THREADS;
    for(auto n_threads = 1; n_threads < max_n_threads; n_threads *= 2) {
        auto start_time = std::chrono::high_resolution_clock::now();
#pragma omp parallel num_threads(n_threads)
        {
            for (int i = 0; i < N_DATA; i++) {
                set.add(i);
            }

            for (int i = 0; i < N_DATA; i++) {
                if(set.contains(i)) {
                    set.remove(i);
                }
            }
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time = end_time - start_time;
        std::cout << "Num threads = " << n_threads << ": ";
        std::cout << time / std::chrono::microseconds (1) << std::endl;
    }
}

int main() {
    std::cout << "Coarse grained " << std::endl;
    auto set = CoarseGrainedSet<int>();
    performance_test(set);
    std::cout << "Fine grained " << std::endl;
    auto other_set = FineGrainedSet<int>();
    performance_test(other_set);
}