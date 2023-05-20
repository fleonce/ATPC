#include <atomic>
#include <iostream>
#include <omp.h>
#include <thread>
#include <cstdint>
#include <chrono>

#define COUNTER_TYPE 3
#define COUNTER_MAX 40000000
#define COUNTER_BACKOFF true
#define BACKOFF_T 10
#define MAX_BACKOFF INT32_MAX

class AtomicCounter {
private:
    int val = 0;
#if COUNTER_TYPE == 0 or COUNTER_TYPE == 1
public:
    int get() {
        return this->val;
    }
#endif
#if COUNTER_TYPE == 0

    // compare_exchange_strong:
    private:
        std::atomic<bool> lock = false;
    public:
        int increment() {
            bool expected = false;
#if COUNTER_BACKOFF
            int backoff = BACKOFF_T;
#endif
            while (not lock.compare_exchange_strong(expected, true)) {
                expected = false;
#if COUNTER_BACKOFF
                std::this_thread::sleep_for(std::chrono::nanoseconds(backoff));
                if(backoff < MAX_BACKOFF) {
                    backoff *= 2;
                }
#endif
            }
            this->val += 1;
            lock.store(false);
            return this->val;
        }
#elif COUNTER_TYPE == 1
    private:
        std::atomic<bool> lock = false;
    public:
        int increment() {
            bool expected = false;
#if COUNTER_BACKOFF
            int backoff = 1;
#endif
            while(not lock.compare_exchange_weak(expected, true)) {
                expected = false;
#if COUNTER_BACKOFF
                std::this_thread::sleep_for(std::chrono::nanoseconds(backoff));
                if(backoff < MAX_BACKOFF) {
                    backoff *= 2;
                }
#endif
            }
            val += 1;
            lock.store(false);
            return val;
        }
#elif COUNTER_TYPE == 2
private:
    std::atomic<int> lock = 0;
public:
    int increment() {
        return lock.fetch_add(1, std::memory_order_acq_rel);
    }

    int get() {
        return lock.load();
    }
#elif COUNTER_TYPE == 3
private:
    std::atomic<int> lock = 0;
public:
    int increment() {
        int old = lock.load();
        int newval = old + 1;

        while(!lock.compare_exchange_strong(old, newval)) {
            newval = old + 1;
        }
        return newval;
    }

    int get() {
        return lock.load();
    }
#endif
};

uint64_t epoch_time() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int main() {
    std::cout << "Backoff: " << COUNTER_BACKOFF << std::endl;
    std::cout << "Counter type: " << COUNTER_TYPE << std::endl;
    AtomicCounter counter = AtomicCounter();

    uint64_t start_t = epoch_time();

#pragma omp parallel for shared(counter)

    for(int i = 0; i < COUNTER_MAX; i++) {
        if(i == 0) {
            std::cout << omp_get_num_threads() << std::endl;
        }
        int val = counter.increment();
        // std::cout << "Counter " << counter.increment() << " " << omp_get_thread_num() << std::endl;
    }

    uint64_t end_t = epoch_time();
    auto diff = (double) (end_t - start_t);
    double throughput = ((double) (COUNTER_MAX)) / diff;
    std::cout << "Throughput: " << throughput << " incr/ms" << std::endl;
    std::cout << "Final value " << counter.get() << std::endl;
}