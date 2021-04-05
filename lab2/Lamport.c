// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

typedef unsigned char byte;

int ID;
int* A;
byte* flag;
int* prio;

int N, M;

int mem_create() {
    ID = shmget(IPC_PRIVATE, sizeof(*A) + N * (sizeof(*flag) + sizeof(*prio)), 0600);
    if (ID == -1) return 0;

    A = (int*)shmat(ID, NULL, 0);
    flag = (byte*)(A + 1);
    prio = (int*)(flag + N);
    return 1;
}

void mem_destroy() {
    shmdt(A);
    shmctl(ID, IPC_RMID, NULL);
    exit(0);
}

void process(int id) {
    for (int i = 0; i < M; i++) {
        // Enter critical section
        flag[id] = 1;

        int max = 0;
        for (int j = 0; j < max; j++)
            if (prio[j] > max) max = prio[j];

        prio[id] = max + 1;
        flag[id] = 0;

        for (int j = 0; j < N; j++) {
            while (flag[j] != 0);
            while (prio[j] != 0 && (prio[j] < prio[id] || (prio[j] == prio[id] && j < id)));
        }

        // Do work
        (*A)++;

        // Leave critical section
        prio[id] = 0;
    }
}

int main(int argc, char* argv[]) {
    N = atoi(argv[1]);
    M = atoi(argv[2]);
    
    if (!mem_create())
        return 1;
    
    sigset(SIGINT, mem_destroy);

    for (int i = 0; i < N;) {
        int f = fork();

        if (f == 0) {
            process(i);
            exit(0);
        
        } else if (f > 0) i++;
    }

    for (int i = 0; i < N; i++)
        wait(NULL);

    printf("A=%d\n", *A);

    mem_destroy(0);
    return 0;
}