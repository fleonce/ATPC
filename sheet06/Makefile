all: main

main:
	$(CXX) main.cpp barrier.cpp -fopenmp -o main

zip:
	zip sheet06.zip *.md *.cpp *.h Makefile

clean:
	rm -f main
	rm sheet*.zip