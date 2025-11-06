#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    int counter = 0;
    long cycle = 0;

    while (1) {
        if (counter % 3 == 0) {
            printf("Process 2 (PID=%d) Cycle: %ld - %d is a multiple of 3\n",
                   getpid(), cycle, counter);
        } else {
            printf("Process 2 (PID=%d) Cycle: %ld\n",
                   getpid(), cycle);
        }

        counter--; 
        cycle++;
        usleep(500000); 
    }

    return 0;
}
