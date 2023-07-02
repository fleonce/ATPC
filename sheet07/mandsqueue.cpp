#include "mandsqueue.h"

template <typename T> ms_queue<T>::ms_queue(T def) {
    ms_node<T> node = ms_node<T>{};
    node.val = nullptr;
    node.next = nullptr;
    counted_ptr<ms_node<T>> counted_ptr;
    counted_ptr.ptr = *node;
    counted_ptr.ctr = 0;
    this->tail = this->head = counted_ptr;
    this->def = def;
}

template <typename T> void ms_queue<T>::enqueue(T val) {
    ms_node<T> node = ms_node<T>{};
    node.val = val;
    node.next = nullptr;
    counted_ptr<ms_node<T>> t, n;
    do {
        t = this->tail.load();
        n = t->next->load();
        if (t == this->tail.load()) {
            if (n.ptr == nullptr) {
                counted_ptr<ms_node<T>> updated_ptr;
                updated_ptr.ptr = *node;
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
    ptr.ptr = *node;
    ptr.ctr = t.ctr + 1;
    this->tail.compare_exchange_strong(t, ptr);
}

template <typename T> T ms_queue<T>::dequeue() {
    counted_ptr<ms_node<T>> h, t, n;
    T ret = this->def;
    do {
        h = this->head.load();
        t = this->tail.load();
        n = h.ptr->next.load();
        if(h == this->head.load()) {
            if(h.ptr == t.ptr) {
                if (n.ptr == nullptr) {
                    return nullptr;
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