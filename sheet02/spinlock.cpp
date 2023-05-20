#include <atomic>
#include <iostream>
#include <omp.h>
#include <thread>

using std::atomic;

template<typename T>
bool compare_exchange_strong2(std::atomic<T>& var, T& expected, T desired) {
    auto previous = expected;
    bool res;
    do {
        res = var.compare_exchange_strong(expected, desired);
    } while(!res && previous == expected);
    return false;
}

class Spinlock {

private:
    std::atomic<bool> flag = false;
public:
    void lock() {
        bool expect = false;
        while (!this->flag.compare_exchange_strong(expect, true)) {
            expect = false;
        }
    }

    void lock_weak() {
        bool expect = false;
        while (!this->flag.compare_exchange_weak(expect, true)) {
            expect = false;
        }
    }

    void unlock() {
        this->flag.store(false);
    }
};


int main() {
    int val = 0;
    bool use_weak = true;
    Spinlock lock = Spinlock();
#pragma omp parallel shared(lock, val)
    {
        int thread_num = omp_get_thread_num();
#pragma omp barrier
        not use_weak ? lock.lock() : lock.lock_weak();
        lock.lock();
        val += 1;
        std::cout << "thread " << thread_num << ": " << val << "\n";
        lock.unlock();
    };
}