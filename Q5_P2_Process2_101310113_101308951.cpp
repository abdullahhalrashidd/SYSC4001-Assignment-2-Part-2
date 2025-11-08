#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

struct SharedData {
    int multiple;
    int counter;
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static void sem_lock(int semid) {
    struct sembuf op = {0, -1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("semop lock failed");
        exit(EXIT_FAILURE);
    }
}

static void sem_unlock(int semid) {
    struct sembuf op = {0, 1, 0};
    if (semop(semid, &op, 1) == -1) {
        perror("semop unlock failed");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    key_t key = ftok("sharedfile", 65);
    if (key == -1) {
        perror("ftok failed");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, sizeof(struct SharedData), 0666);
    if (shmid < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    }

    struct SharedData *shared =
        (struct SharedData *)shmat(shmid, NULL, 0);
    if (shared == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }

    int semid = semget(key, 1, 0666);
    if (semid < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_lock(semid);
        int current = shared->counter;
        sem_unlock(semid);

        if (current > 100)
            break;

        usleep(200000);
    }

    while (1) {
        sem_lock(semid);

        if (shared->counter > 500) {
            sem_unlock(semid);
            break;
        }

        printf("Process 2 (PID=%d): counter = %d (multiple = %d)\n", getpid(), shared->counter, shared->multiple);

        sem_unlock(semid);
        fflush(stdout);
        usleep(300000);
    }

    if (shmdt(shared) == -1)
        perror("shmdt failed");

    printf("Process 2 (PID=%d): finished reading shared memory.\n", getpid());
    return 0;
}
