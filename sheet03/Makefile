all: lock peterson

peterson:
	g++ peterson_lock.cpp -fopenmp -o peterson_lock

lock:
	g++ mcs_lock.cpp -fopenmp -o mcs_lock

zip:
	zip -f sheet03.zip *.md *.cpp Makefile

clean:
	rm -f mcs_lock peterson_lock