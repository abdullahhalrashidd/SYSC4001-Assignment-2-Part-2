#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct SharedData {
    int multiple;   
    int counter;    
};

int main(void) {
    
    key_t key = ftok("sharedfile", 65);             
    int shmid = shmget(key, sizeof(struct SharedData), 0666 | IPC_CREAT);
    if (shmid < 0) { perror("shmget failed"); exit(EXIT_FAILURE); }

    struct SharedData *shared = (struct SharedData *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) { perror("shmat failed"); exit(EXIT_FAILURE); }

    
    shared->multiple = 3;
    shared->counter  = 0;

    
    pid_t pid = fork();
    if (pid < 0) { perror("fork failed"); exit(EXIT_FAILURE); }

    if (pid == 0) {
        execl("./Q4_P2_Process2_101310113_101308951",
              "Q4_P2_Process2_101310113_101308951",
              (char *)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

    
    while (shared->counter <= 500) {
        if (shared->counter % shared->multiple == 0)
            printf("Process 1 (PID=%d): %d IS a multiple of %d\n",
                   getpid(), shared->counter, shared->multiple);
        else
            printf("Process 1 (PID=%d): counter = %d\n",
                   getpid(), shared->counter);

        shared->counter++;
        usleep(200000);  
    }

    
    int status = 0;
    waitpid(pid, &status, 0);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);

    printf("Process 1 (PID=%d): shared memory removed, child status = %d\n",
           getpid(), status);
    return 0;
}