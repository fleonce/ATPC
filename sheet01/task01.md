### Task 1

(a) Consistency models:

1. Model
   - SC: `print(val);` always prints 42, since there is no sequential order of execution in which flag gets set to 1 before the change of val to 42.
   - TSO: `print(val);` can only return 42, as all orderings besides reads overtaking writes are respected, but W||W is implied.
   - Relaxed: `print(val)` can return both 42 and 0, as no limit on reordering is given here.
   
2. Model
   - SC: `print(y)` can return 1, can return 0 if `print(x)` returns 1, `print(x)` can return 0 if `y` returns 1 but not both can be 0.
   - TSO: `print(y)` and `print(x)` can both return 0 at the same time, since the read of shared variable `x` can get read before writing to `y` happens.
   - Relaxed: Same as for TSO.

3. Model:
    - SC: Any outcome is possible, since there are no constraints inside the code leading to any dependencies between the executing threads. Both thread `1` and `2` can print all 4 combinations of `0` and `1`.
   - SC: Not both threads can read 0,1, the same goes for TSO.
   - TSO & Relaxed: same story 

(b)

- TSO:
  1. For example 1, nothing has to be done
  2. In example 2, we have to add x.store(1, ||R), to prevent reads overtaking our write.

- Relaxed:
  1. In example 1, we have to add flag.store(1, W||), flag.loaad(||RW)
  2. Add the same as in TSO.

- TSO: For TSO, adding fences will do the trick, since the only "limitation" of TSO is that reads may overtake writes. If fences get placed after writes, reads cannot overtake them anymore. The same counts for synchronizing writes, since they specify which kind of reorderings are disallowed.
- RC: Since relaxed consistency offers no guarantees at all, a combination of fences and synchronizing reads should do the trick.

(c)

```
gcc sheet01/sc.c -o sheet01/sc && ./sheet01/sc
1: 0
1: 0
2: 0
2: 0

```
