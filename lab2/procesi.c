// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

int ID;
int* A;

int M;

int mem_create() {
    ID = shmget(IPC_PRIVATE, sizeof(*A), 0600);
    if (ID == -1) return 0;

    A = (int*)shmat(ID, NULL, 0);
    return 1;
}

void mem_destroy() {
    shmdt(A);
    shmctl(ID, IPC_RMID, NULL);
    exit(0);
}

void process() {
    for (int i = 0; i < M; i++)
        (*A)++;
}

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]);
    M = atoi(argv[2]);
    
    if (!mem_create())
        return 1;
    
    sigset(SIGINT, mem_destroy);

    for (int i = 0; i < N;) {
        int f = fork();

        if (f == 0) {
            process();
            exit(0);
        
        } else if (f > 0) i++;
    }

    for (int i = 0; i < N; i++)
        wait(NULL);

    printf("A=%d\n", *A);

    mem_destroy(0);
    return 0;
}