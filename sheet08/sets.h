#ifndef ATPC_SETS_H
#define ATPC_SETS_H

#include <limits>
#include "lock.h"


template<typename T>
class Node
{
public:
    T val;
    Node<T> *next = nullptr;

    Node(const T& v) : val(v) {}
    Node(const T& v, const Node<T> *p) : val(v), next(p) {}
};

template <typename T>
class AtomicNode
{
public:
    T val;
    AtomicNode<T>* next = nullptr;
    RAII_LOCK_T _lock;

    explicit AtomicNode(const T& v) : val(v), _lock() {}
    AtomicNode(const T& v, const AtomicNode<T> *p): val(v), next(p), _lock() {}

    RAII raii() {
        return RAII(&this->_lock);
    }

    void acquire() {
        this->_lock.acquire();
    }

    void release() {
        this->_lock.release();
    }
};

template<typename T>
class Set {

public:
    Set() = default;
    virtual ~Set() = default;

    virtual bool add(const T &val) = 0;
    virtual bool remove(const T &val) = 0;
    virtual bool contains(const T &val) = 0;
    virtual void dump() = 0;
};

template<typename T>
class FineGrainedSet: public Set<T> {
private:
    AtomicNode<T> tail;
    AtomicNode<T> head;
public:
    FineGrainedSet() : head(std::numeric_limits<T>::min()),
                       tail(std::numeric_limits<T>::max())
    {
        head.next = &tail;
    }

    AtomicNode<T> * find_node_hoh(AtomicNode<T>* start, const T &val);
    bool add(const T &val) override;
    bool remove(const T &val) override;
    bool contains(const T &val) override;
    void dump() override;
};

template<typename T>
class CoarseGrainedSet: public Set<T> {
protected:

private:
    Node<T> tail;
    Node<T> head;
    RAII_LOCK_T _lock;
public:
    CoarseGrainedSet() : head(std::numeric_limits<T>::min()),
                         tail(std::numeric_limits<T>::max()),
                         _lock()
    {
        head.next = &tail;
    };
    RAII lock();

    Node<T> *find_node(const T &val);
    bool add(const T &val) override;
    bool remove(const T &val) override;
    bool contains(const T &val) override;
    void dump() override;
};


#endif //ATPC_SETS_H
