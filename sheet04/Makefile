all: shared_fairness test

shared_fairness:
	g++ fairness_shared_mutex.cpp -fopenmp -o fairness_shared_mutex

test:
	g++ test_and_set_lock.cpp test_performance.cpp mcs_lock.cpp -fopenmp -o test_perf

zip:
	zip sheet04.zip *.md *.cpp *.h Makefile

clean:
	rm -f fairness_shared_mutex