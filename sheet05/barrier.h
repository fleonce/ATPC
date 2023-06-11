#include <atomic>

#ifndef ATPC_BARRIER_H
#define ATPC_BARRIER_H

class Barrier {
private:
    std::atomic_int ctr = 0;
    std::atomic_int epoch = 0;
    std::atomic_int flag{0};
public:
    int arrive();
};

#endif //ATPC_BARRIER_H
