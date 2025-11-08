
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>

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

    int shmid = shmget(key, sizeof(struct SharedData), 0666 | IPC_CREAT);
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

    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    union semun arg;
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL failed");
        exit(EXIT_FAILURE);
    }

    sem_lock(semid);
    shared->multiple = 3;
    shared->counter  = 0;
    sem_unlock(semid);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execl("./Q5_P2_Process2_101310113_101308951", "Q5_P2_Process2_101310113_101308951", (char *)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        sem_lock(semid);

        if (shared->counter > 500) {
            sem_unlock(semid);
            break;
        }

        if (shared->counter % shared->multiple == 0) {
            printf("Process 1 (PID=%d): %d IS a multiple of %d\n", getpid(), shared->counter, shared->multiple);
        } else {
            printf("Process 1 (PID=%d): counter = %d\n", getpid(), shared->counter);
        }

        shared->counter++;

        sem_unlock(semid);
        fflush(stdout);
        usleep(200000);
    }

    int status = 0;
    waitpid(pid, &status, 0);

    if (shmdt(shared) == -1)
        perror("shmdt failed");

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        perror("shmctl IPC_RMID failed");

    if (semctl(semid, 0, IPC_RMID) == -1)
        perror("semctl IPC_RMID failed");

    printf("Process 1 (PID=%d): finished, child status = %d\n",
           getpid(), status);
    return 0;
}
