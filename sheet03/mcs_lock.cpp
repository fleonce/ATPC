#include <atomic>
#include <omp.h>
#include <iostream>
#include <thread>
#include <cassert>

struct q_node {
    std::atomic<q_node*> next{nullptr};
    std::atomic_bool wait{false};
};

class MCSLock {
private:
    std::atomic<q_node*> tail = nullptr;
public:
    void acquire(q_node* p) {
        p->wait.store(true); // can be relaxed ordering
        p->next.store(nullptr); // this aswell

        // because of this
        q_node* prev = tail.exchange(p, std::memory_order_acq_rel);
        if(prev != nullptr) {
            prev->next.store(p); // can be relaxed
            while(p->wait.load(std::memory_order_acquire));

            std::atomic_thread_fence(std::memory_order_release);
        }
    }

    void release(q_node* p) {
        q_node* succ = p->next.load(std::memory_order_release);
        if(succ == nullptr) {
            q_node* cmp = p;
            q_node* change_to = nullptr;
            if(tail.compare_exchange_strong(cmp, change_to)) {
                return;
            }
            // cmp now holds pointer to the tail,
            // so we have to wait until succ is updated to something non-null
            do {
                succ = p->next.load();
            } while(succ == nullptr);
        }
        succ->wait.store(false, std::memory_order_release);
    }
};



int main() {
    MCSLock lock = MCSLock();
    int val = 0;

#pragma omp barrier
#pragma omp parallel shared(lock, val)
    {
        q_node entry{};
        lock.acquire(&entry);
        val += 1;
        lock.release(&entry);
    };

    assert((val == omp_get_num_threads()) && "Value must be equal to the thread number");
    std::cout << "Final value: " << val << std::endl;

    val = 0;
    int loop_max = 10000;
#pragma omp parallel for shared(lock, val) private(loop_max)
    for(int i = 0; i < loop_max; i++) {
        q_node node{};
        lock.acquire(&node);
        val += 1;
        lock.release(&node);
    }

    assert((val == loop_max));
    std::cout << "Final value: " << val << std::endl;
}