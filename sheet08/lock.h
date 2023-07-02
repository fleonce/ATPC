#ifndef ATPC_LOCK_H
#define ATPC_LOCK_H

#include <atomic>
#include <chrono>
#include <thread>

#define RAII_LOCK_T TASLock

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

class RAII {
private:
    RAII_LOCK_T* lock;
    bool locked = true;
public:
    explicit RAII(RAII_LOCK_T *t)
    {
        this->lock = t;
        this->lock->acquire();
    }

    ~RAII()
    {
        this->release();
    }

    void release() {
        if (this->locked) {
            this->lock->release();
            this->locked = false;
        }
    }
};

#endif //ATPC_LOCK_H
