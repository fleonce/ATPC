#include <atomic>
#include <omp.h>
#include <cassert>
#include <iostream>

#define MAX_VAL 100000000
#define MEMORY_ORDER_LOAD std::memory_order_acquire
#define MEMORY_ORDER_STORE std::memory_order_release

class PetersonLock {
private:
    std::atomic_int turn{0};
    std::atomic_bool interested[2]{false, false};
public:
    void acquire() {
        assert((omp_get_num_threads() == 2) && "Thread number too high");
        int self = omp_get_thread_num();
        int other = 1 - self;
        interested[self].store(true, MEMORY_ORDER_STORE);
        turn.store(other, MEMORY_ORDER_STORE);

        while(interested[other].load(MEMORY_ORDER_LOAD) && turn.load(MEMORY_ORDER_LOAD) == other);
    }

    void release() {
        interested[omp_get_thread_num()].store(false, MEMORY_ORDER_STORE);
    }
};

int main() {
    int val = 0;
    PetersonLock lock = PetersonLock();
#pragma omp barrier
#pragma omp parallel for shared(val)
    for(int i = 0; i < MAX_VAL; i++) {
        lock.acquire();
        val += 1;
        lock.release();
    }
#pragma omp barrier
    std::cout << "Final value: " << val << std::endl;
}