#ifndef ATPC_LOCK_H
#define ATPC_LOCK_H

#include <atomic>

struct q_node {
    std::atomic<q_node*> next{nullptr};
    std::atomic_bool wait{false};
};

class MCSLock {
private:
    std::atomic<q_node*> tail = nullptr;
public:
    void acquire();

    void release();
};

class TASLock {
private:
    std::atomic_bool free = {false};
public:
    void acquire();

    void release();
};

class TTASLock {
private:
    std::atomic_bool free = {false};
public:
    void acquire();

    void release();
};

#endif //ATPC_LOCK_H
