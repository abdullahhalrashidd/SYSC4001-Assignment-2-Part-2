#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid;

    pid = fork();   

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        
        int counter = 0;
        while (1) {
            printf("Child Process (PID=%d) Counter: %d\n", getpid(), counter++);
            usleep(500000);   
        }
    } else {
        
        int counter = 0;
        while (1) {
            printf("Parent Process (PID=%d) Counter: %d\n", getpid(), counter++);
            usleep(500000);   
        }
    }

    return 0;   
}