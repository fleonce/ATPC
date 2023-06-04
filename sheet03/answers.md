### Task 1

(a)

- The lock still offers mutual exclusion, if the lock is unavailable, at least 1 is interested, and it is his turn. 
- The lock however is not starvation free, one process can permanently acquire the lock.
#### Solution

- The lock does not offer mutual exclusion if the threads acquire the lock at different times. If one already holds the lock, the other will be also able to acquire the lock.
- The lock is not starvation free, because one thread can acquire the lock again before the second thread can acquire the lock (f.e. because it is waiting before checking turn again)

(b)