#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();
    if (pid < 0) { perror("fork failed"); exit(EXIT_FAILURE); }
    if (pid == 0) {
        execl("./Q3_P2_Process2_101310113_101308951", "Q3_P2_Process2_101310113_101308951", (char*)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }
    int status = 0;
    if (waitpid(pid, &status, 0) < 0) { perror("waitpid failed"); exit(EXIT_FAILURE); }
    printf("Process 1 (PID=%d): child exited (status=%d). Exiting.\n", getpid(), status);
    return 0;
}
