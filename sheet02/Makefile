all: spinlock atomic

atomic:
	g++ atomic_counters.cpp -fopenmp -o atomic_counters

spinlock:
	g++ spinlock.cpp -fopenmp -o spinlock

clean:
	rm -f spinlock atomic_counters