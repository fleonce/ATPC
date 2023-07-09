#include <atomic>
#include <cstdlib>
#include "omp.h"

template<typename T>
class Node {
    T val;
    Node<T> * next;
};

template<typename T>
struct CountedPtr {
    Node<T> *ptr = nullptr;
    unsigned count = 0;

    CountedPtr(Node<T> * ptr, unsigned count) {
        this->ptr = ptr;
        this->count = count;
    }

    CountedPtr() {
        this->ptr = nullptr;
        this->count = 0;
    }
};

template<typename T>
class Stack {
private:
    T nil;
    std::atomic<CountedPtr<T>> top = CountedPtr<T>();
    std::atomic<Node<T> *> hazard_ptr[];
public:
    explicit Stack(int world_size, T nil) {
        this->hazard_ptr = new std::atomic<Node<T> *>[world_size];
        this->nil = nil;
    };
    T pop();
    void push(T t);
    void retire(Node<T> * node);
};

template<typename T> void Stack<T>::retire(Node<T> *node) {
    bool non_retired = false;
    for (int i = 0; i < omp_get_num_threads(); i++) {
        std::atomic<Node<T>*> x = this->hazard_ptr[i];
        if(x.load()) {
            non_retired = true;
            break;
        }
    }

    if (non_retired) {
        return;
    }
    free(node);
}

template<typename T> T Stack<T>::pop() {
    CountedPtr<T> oldtop, newtop;
    while (true) {
        oldtop = this->top.load();
        this->hazard_ptr[omp_get_thread_num()] = oldtop->ptr;
        if (oldtop != this->top.load())
            continue;
        newtop = CountedPtr(oldtop.ptr, oldtop.count + 1);

        if(this->top.compare_exchange_strong(oldtop, newtop)) {
            break;
        }
    }
    this->hazard_ptr[omp_get_thread_num()] = nullptr;
    if (oldtop.ptr) {
        T val = oldtop.ptr->val;
        this->retire(oldtop);
        return val;
    }
    return this->nil;
}

template<typename T> void Stack<T>::push(T t) {
    CountedPtr<T> oldtop, newtop;
    Node<T> * node;
    node->val = t;
    while (true) {
        oldtop = this->top.load();
        node->next = oldtop.ptr;
        newtop = CountedPtr(node, oldtop.count);
        if(this->top.compare_exchange_strong(oldtop, newtop)) {
            break;
        }
    }
}

int main() {
    return 0;
}