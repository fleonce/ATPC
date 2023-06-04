#include "lock.h"

void TASLock::acquire() {
    bool expect = false;
    while (!this->free.compare_exchange_strong(expect, true)) {
        expect = false;
    }
}

void TASLock::release() {
    this->free.store(false, std::memory_order_release);
}

void TTASLock::acquire() {
    bool expect = false;
    while(!this->free.compare_exchange_strong(expect, true)) {
        while(!this->free.load(std::memory_order_relaxed)) {}
    }
}

void TTASLock::release() {
    this->free.store(false, std::memory_order_release);
}