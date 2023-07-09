DEPS = lock.h set.h
CFLAGS =
OBJ_FILES := $(wildcard *.o)
SRC_FILES = $(wildcard *.cpp)

all: treiber_stack hazard_treiber_stack

treiber_stack: treiber_stack.cpp
	$(CXX) -o $@ $^ $(CFLAGS)

hazard_treiber_stack: hazard_treiber_stack.cpp
	$(CXX) -o $@ $^ $(CFLAGS)

zip:
	zip $(shell basename "$(shell pwd)").zip *.md *.cpp *.h Makefile

clean:
	rm -f *.o tests
	rm -f sheet*.zip

.PHONY: clean
