#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    int counter = 0;
    long cycle = 0;
    while (1) {
        printf("Process 2 (PID=%d) Cycle: %ld - %d %s a multiple of 3\n", getpid(), cycle, counter, (counter % 3 == 0) ? "IS" : "is NOT");
        counter--;          
        cycle++;
        if (counter < -500)
            break;
    }
    return 0;
}
