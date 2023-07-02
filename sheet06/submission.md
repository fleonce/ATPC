## Sheet 06

#### Task 1

T1: q1.enqueue(A) q2.enqueue(C) q2.dequeue()

T2: q2.enqueue(B) q1.enqueue(D) q1.dequeue()

Since both q1,q2 are seq. consistent, q1.dequeue() can return D, q2.dequeue() can return C

This implies however that q1.enqueue(D) happened before q1.enqueue(A) 
which in turn implies that q2.enqueue(C) happened after q1.enqueue(D)
but that does not hold because q2.dequeue() returned C, so there is an ordering loop
from the end of T2 to the start of T1 and vice versa.

#### Task 2

- i) Linearizability
- ii) Seq. Consistency 
- iii) Seq. Consistency 
