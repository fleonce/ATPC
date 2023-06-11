all: main

main:
	$(CXX) main.cpp barrier.cpp -fopenmp -o main

zip:
	zip sheet05.zip *.md *.cpp *.h Makefile

clean:
	rm -f main