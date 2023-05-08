#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    void* shm = mmap(NULL, 2 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int *data = (int*) shm;
    data[0] = 0;
    data[1] = 0;
    for (int i = 0; i < 4; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (i == 0) {
                data[1] = 1;
            } else if (i == 1) {
                printf("1: %d\n", data[0]);
                printf("1: %d\n", data[1]);
            } else if (i == 2) {
                printf("2: %d\n", data[1]);
                printf("2: %d\n", data[0]);
            } else if (i == 3) {
                data[0] = 1;
            }
            exit(0);
        }
    }
    munmap(shm, 2 * sizeof (int));
    return 0;
}

/*
void foo(int c, int &x, int &y) {
    if(c == 0) {
        x = 3;
    } else {
        y = 4;
    }
    int y1 = y;
    int x1 = x;
    std::cout << "X: " << x1 << " Y: " << y1 << "\n";
}*/

int main1() {
    /*
    for (int j = 0; j < 10000; j++) {
        int x = 0;
        int y = 0;
        std::thread first (foo, 0, std::ref(x), std::ref(y));
        std::thread second (foo, 1, std::ref(x), std::ref(y));

        first.join();
        second.join();
    }*/
    return 0;
}