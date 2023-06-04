#include <atomic>
#include "lock.h"

thread_local q_node active_node = {};

q_node *thread_local_q_node() {
    return &active_node;
}

void MCSLock::acquire() {
    q_node *p = thread_local_q_node();
    p->wait.store(true); // can be relaxed ordering
    p->next.store(nullptr); // this aswell

    // because of this
    q_node *prev = tail.exchange(p, std::memory_order_acq_rel);
    if (prev != nullptr) {
        prev->next.store(p); // can be relaxed
        while (p->wait.load(std::memory_order_acquire));

        std::atomic_thread_fence(std::memory_order_release);
    }
}

void MCSLock::release() {
    q_node *p = thread_local_q_node();
    q_node *succ = p->next.load(std::memory_order_release);
    if (succ == nullptr) {
        q_node *cmp = p;
        q_node *change_to = nullptr;
        if (tail.compare_exchange_strong(cmp, change_to)) {
            return;
        }
        // cmp now holds pointer to the tail,
        // so we have to wait until succ is updated to something non-null
        do {
            succ = p->next.load();
        } while (succ == nullptr);
    }
    succ->wait.store(false, std::memory_order_release);
}