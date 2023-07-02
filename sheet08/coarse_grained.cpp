#include <iostream>
#include "lock.h"
#include "sets.h"

template class CoarseGrainedSet<int>;

template<typename T>
RAII CoarseGrainedSet<T>::lock() {
    return RAII(&this->_lock);
}

template<typename T>
void CoarseGrainedSet<T>::dump() {
    auto mut = this->lock();
    Node<T> *node = &this->head;
    while (node) {
        std::cout << node << " " << node->val << "\n";
        node = node->next;
    }
    std::cout << "------------------------\n\n";
}

template<typename T>
Node<T> *CoarseGrainedSet<T>::find_node(const T &val) {
    Node<T> *curr = &this->head;
    Node<T> *next = curr->next;

    while (next && next->val < val) {
        curr = next;
        next = curr->next;
    }
    return curr;
}

template<typename T>
bool CoarseGrainedSet<T>::add(const T &val) {
    auto mut = this->lock();
    Node<T> *res = find_node(val);
    if (res->next->val == val)
        return false;

    auto *node = new Node<T>(val);
    node->next = res->next;
    res->next = node;
    return true;
}

template<typename T>
bool CoarseGrainedSet<T>::remove(const T &val) {
    auto mut = this->lock();
    Node<T> *res = find_node(val);
    if (res->next->val != val) {
        mut.release();
        return false;
    }

    Node<T> *node = res->next;
    res->next = node->next;
    delete (node);
    mut.release();
    return true;
}

template<typename T>
bool CoarseGrainedSet<T>::contains(const T &val) {
    auto mut = this->lock();
    Node<T> *res = find_node(val);
    bool result = res->next->val == val;
    mut.release();
    return result;
}