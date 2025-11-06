#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execl("./Q2_P2_Process2_101310113_101308951", "Q2_P2_Process2_101310113_101308951", (char *)NULL);

        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {
        int counter = 0;
        long cycle = 0;

        while (1) {
            if (counter % 3 == 0) {
                printf("Process 1 (PID=%d) Cycle: %ld - %d is a multiple of 3\n",
                       getpid(), cycle, counter);
            } else {
                printf("Process 1 (PID=%d) Cycle: %ld\n",
                       getpid(), cycle);
            }

            counter++;
            cycle++;
            usleep(500000);
        }
    }

    return 0;
}
