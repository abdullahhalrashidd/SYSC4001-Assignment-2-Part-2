#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

using namespace std;

struct SharedData {
    int multiple;
    int counter;
};


void sem_wait(int semid) {
    struct sembuf op = {0, -1, 0};
    semop(semid, &op, 1);
}
void sem_signal(int semid) {
    struct sembuf op = {0, 1, 0};
    semop(semid, &op, 1);
}

int main() {
    key_t shmKey = ftok("sharedfile", 65);
    key_t semKey = ftok("semfile", 75);

   
    int shmid = shmget(shmKey, sizeof(SharedData), 0666);
    if (shmid < 0) { perror("shmget failed"); exit(EXIT_FAILURE); }

    SharedData* shared = static_cast<SharedData*>(shmat(shmid, nullptr, 0));
    if (shared == reinterpret_cast<void*>(-1)) { perror("shmat failed"); exit(EXIT_FAILURE); }

  
    int semid = semget(semKey, 1, 0666);
    if (semid < 0) { perror("semget failed"); exit(EXIT_FAILURE); }

   
    while (true) {
        sem_wait(semid);
        int c = shared->counter;
        sem_signal(semid);

        if (c > 100) break;
        usleep(200000);
    }

   
    while (true) {
        sem_wait(semid);

        if (shared->counter > 500) {
            sem_signal(semid);
            break;
        }

        cout << "Process 2 (PID=" << getpid() << "): counter = "
             << shared->counter << " (multiple = "
             << shared->multiple << ")" << endl;

        sem_signal(semid);
        usleep(300000);
    }

    shmdt(shared);
    cout << "Process 2 (PID=" << getpid()
         << "): finished reading shared memory." << endl;
    return 0;
}