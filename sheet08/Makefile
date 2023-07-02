DEPS = lock.h set.h
CFLAGS =
OBJ_FILES := $(wildcard *.o)
SRC_FILES = $(wildcard *.cpp)

all: coarse_grained.cpp

coarse_grained: coarse_grained.cpp
	$(CXX) -o $@ $^ $(CFLAGS)

fine_grained: fine_grained.cpp
	$(CXX) -o $@ $^ $(CFLAGS)

tests: lock.h sets.h coarse_grained.cpp fine_grained.cpp test_and_set_lock.cpp test_sets.cpp
	$(CXX) -fopenmp -o $@ $^ $(CFLAGS)

zip:
	zip $(shell basename "$(shell pwd)").zip *.md *.cpp *.h Makefile

clean:
	rm -f *.o tests
	rm -f sheet*.zip

.PHONY: clean
