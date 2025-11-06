#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct SharedData {
    int multiple;
    int counter;
};

int main(void) {

    key_t key = ftok("sharedfile", 65);
    int shmid = shmget(key, sizeof(struct SharedData), 0666);
    if (shmid < 0) { perror("shmget failed"); exit(EXIT_FAILURE); }

    struct SharedData *shared = (struct SharedData *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) { perror("shmat failed"); exit(EXIT_FAILURE); }


    while (shared->counter <= 100)
        usleep(200000);

   
    while (shared->counter <= 500) {
        printf("Process 2 (PID=%d): counter = %d (multiple = %d)\n",
               getpid(), shared->counter, shared->multiple);
        usleep(300000);
    }

    shmdt(shared);
    printf("Process 2 (PID=%d): finished reading shared memory.\n", getpid());
    return 0;
}