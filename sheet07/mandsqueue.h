
#include <atomic>

#ifndef ATPC_MANDSQUEUE_H
#define ATPC_MANDSQUEUE_H


template <typename T> class counted_ptr {
public:
    T* ptr = nullptr;
    int ctr = 0;
};

template <typename T> bool operator==(counted_ptr<T> a, counted_ptr<T> b) {
    return a.ctr == b.ctr and a.ptr == b.ptr;
}

template <typename T> struct ms_node {
public:
    T val;
    std::atomic<counted_ptr<ms_node<T>>> next;
};

template <typename T> class ms_queue {
private:
    std::atomic<counted_ptr<ms_node<T>>> head, tail;
    T def;
    counted_ptr<ms_node<T>> null;

public:
    explicit ms_queue(T def) {
        counted_ptr<ms_node<T>> counted_ptr, null_ptr;
        null_ptr.ptr = nullptr;
        null_ptr.ctr = 0;
        this->null = null_ptr;
        ms_node<T> node{};
        node.val = this->def;
        node.next = this->null;
        counted_ptr.ptr = &node;
        counted_ptr.ctr = 0;
        this->tail = this->head = counted_ptr;
        this->def = def;
    }

    void enqueue(T val) {
        ms_node<T> node{};
        node.val = val;
        node.next = this->null;
        counted_ptr<ms_node<T>> t, n;
        do {
            t = this->tail.load();
            n = t.ptr->next.load();
            if (t == this->tail.load()) {
                if (n.ptr == nullptr) {
                    counted_ptr<ms_node<T>> updated_ptr;
                    updated_ptr.ptr = &node;
                    updated_ptr.ctr = n.ctr + 1;
                    if(t.ptr->next.compare_exchange_strong(n, updated_ptr)) {
                        break;
                    }
                } else {
                    counted_ptr<ms_node<T>> ptr;
                    ptr.ptr = n.ptr;
                    ptr.ctr = t.ctr + 1;
                    this->tail.compare_exchange_strong(t, ptr);
                }
            }
        } while (true);
        counted_ptr<ms_node<T>> ptr;
        ptr.ptr = &node;
        ptr.ctr = t.ctr + 1;
        this->tail.compare_exchange_strong(t, ptr);
    }

    T dequeue() {
        counted_ptr<ms_node<T>> h, t, n;
        T ret = this->def;
        do {
            h = this->head.load();
            t = this->tail.load();
            n = h.ptr->next.load();
            if(h == this->head.load()) {
                if(h.ptr == t.ptr) {
                    if (n.ptr == nullptr) {
                        return this->def;
                    }
                    counted_ptr<ms_node<T>> upt;
                    upt.ptr = n.ptr;
                    upt.ctr = t.ctr + 1;
                    this->tail.compare_exchange_strong(t, upt);
                } else {
                    ret = n.ptr->val;
                    counted_ptr<ms_node<T>> upt;
                    upt.ptr = n.ptr;
                    upt.ctr = h.ctr + 1;
                    if(this->head.compare_exchange_strong(h, upt)) {
                        break;
                    }
                }
            }
        } while(true);

        return ret;
    }
};

#endif //ATPC_MANDSQUEUE_H
