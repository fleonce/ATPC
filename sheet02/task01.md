### Task 1

- (b): No it cannot, as there will be a data race on the flag variable. If two threads A and B read flag to be `false`, both will assume the lock is free. They cannot "negotiate" who of the both now should actually hold the lock.
- Corrected: Yes, you can, see Peterson Lock

### Task 2

- (a): No difference to be seen unfortunately :(
- (b): -

### Task 3

Findings: 
- no diff between weak and strong exchange, backoff makes huge difference, fetch_add is fastest.
- backoff time close to 1 is the best



