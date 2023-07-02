#include <iostream>
#include "sets.h"

template class FineGrainedSet<int>;

template<typename T>
AtomicNode<T> * FineGrainedSet<T>::find_node_hoh(AtomicNode<T>* start, const T &val) {
    AtomicNode<T> * current = start;
    current->acquire();
    AtomicNode<T> * next = current->next;
    next->acquire();
    while(next && next->val < val) {
        current->release();
        current = next;
        next = current->next;
        next->acquire();
    }
    return current;
}

template <typename T>
void FineGrainedSet<T>::dump() {
    AtomicNode<T> * node = &this->head;
    node->acquire();
    AtomicNode<T> *next = node->next;
    next->acquire();
    while(next) {
        std::cout << next << " " << next->val << std::endl;
        node->release();
        node = next;
        next = node->next;
        next->acquire();
    }
    std::cout << "------------------------\n\n";
    node->release();
    next->release();
}

template <typename T>
bool FineGrainedSet<T>::contains(const T &val) {
    AtomicNode<T> * res = this->find_node_hoh(&this->head, val);
    bool result = res->next->val == val;
    res->release();
    res->next->release();
    return result;
}

template <typename T>
bool FineGrainedSet<T>::add(const T &val) {
    AtomicNode<T> * res = this->find_node_hoh(&this->head, val);
    if (res->next->val == val) {
        res->release();
        res->next->release();
        return false;
    }

    auto * node = new AtomicNode<T>(val);
    node->next = res->next;
    res->next = node;
    res->release();
    node->next->release();
    return true;
}

template<typename T>
bool FineGrainedSet<T>::remove(const T &val) {
    AtomicNode<T> * res = this->find_node_hoh(&this->head, val);
    if(res->next->val != val) {
        res->release();
        res->next->release();
        return false;
    }

    AtomicNode<T> * next = res->next;
    res->next = next->next;
    res->release();
    next->release();
    delete(next);
    return true;
}