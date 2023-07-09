#include <atomic>

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
    std::atomic<CountedPtr<T>> top = CountedPtr<T>();

    Node<T> * pop();
    void push(Node<T> * n);
};

template<typename T> Node<T> * Stack<T>::pop() {
    CountedPtr<T> oldtop, newtop;
    while (true) {
        oldtop = this->top.load();
        newtop = CountedPtr(oldtop.ptr, oldtop.count + 1);
        if(this->top.compare_exchange_strong(oldtop, newtop)) {
            break;
        }
    }
    return oldtop.ptr;
}

template<typename T> void Stack<T>::push(Node<T> * n) {
    CountedPtr<T> oldtop, newtop;
    while (true) {
        oldtop = this->top.load();
        n->next = oldtop ? oldtop.ptr : nullptr;
        newtop = CountedPtr(n, oldtop.count);
        if(this->top.compare_exchange_strong(oldtop, newtop)) {
            break;
        }
    }
}

int main() {
    return 0;
}