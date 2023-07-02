all: ll consensus

ll:
	$(CXX) llsc.cpp -o llsc

consensus:
	$(CXX) -fopenmp mandsqueue.h mandsqueue.cpp consensus.cpp -o cons

zip:
	zip sheet07.zip *.md *.cpp *.h Makefile

clean:
	rm -f cas
	rm -f sheet*.zip