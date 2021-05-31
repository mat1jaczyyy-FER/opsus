// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"
#pragma GCC diagnostic ignored "-Wformat-security"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define SEM_K    (0)
#define SEM_DJED (1)
#define SEM_PAT  (2)
#define SEM_SOB  (3)

int sem_id;

int sem_create() {
    sem_id = semget(IPC_PRIVATE, 4, 0600);
    if (sem_id == -1) return 0;

    if (semctl(sem_id, SEM_K,    SETVAL, 1) == -1) return 0;
    if (semctl(sem_id, SEM_DJED, SETVAL, 0) == -1) return 0;
    if (semctl(sem_id, SEM_PAT,  SETVAL, 0) == -1) return 0;
    if (semctl(sem_id, SEM_SOB,	 SETVAL, 0) == -1) return 0;

    return 1;
}

void sem_op(int n, int v) {  
   struct sembuf s = {n, v, 0};
   semop(sem_id, &s, 1);
}

void sem_wait(int n) { sem_op(n, -1); }
void sem_set(int n)  { sem_op(n, 1); }

void sem_destroy() { 
    semctl(sem_id, 0, IPC_RMID);
}

struct data {
    int n_sob;
    int n_pat;
};

#define SOB_MAX (10)
#define PAT_MAX (3)

struct data* d;

int mem_id;

int mem_create() {
    mem_id = shmget(IPC_PRIVATE, sizeof(*d), 0600);
    if (mem_id == -1) return 0;

    d = (struct data*)shmat(mem_id, NULL, 0);
    return 1;
}

void mem_destroy() {
    shmdt(d);
    shmctl(mem_id, IPC_RMID, NULL);
}

void action(const char* a) {
    printf(a);
    printf("...\n");

    sleep(2);
    printf("^gotovo\n");
}

void djed() {
    while (1) {
        printf(" djed spava...\n");

        sem_wait(SEM_DJED);
        sem_wait(SEM_K);

        printf(" djed probuden\n");

        if (d->n_sob == SOB_MAX && d->n_pat > 0) {
            sem_set(SEM_K);

            action("ukrcaj_poklone_i_raznosi");

            sem_wait(SEM_K);

            sem_op(SEM_SOB, SOB_MAX);
            d->n_sob = 0;
        }

        if (d->n_sob == SOB_MAX) {
            sem_set(SEM_K);

            action("nahrani_sobove");

            sem_wait(SEM_K);
        }
        
        while (d->n_pat >= PAT_MAX) {
            sem_set(SEM_K);

            action("rijesi_njihov_problem");

            sem_wait(SEM_K);

            sem_op(SEM_PAT, PAT_MAX);
            d->n_pat -= PAT_MAX;
        }

        sem_set(SEM_K);
    }
}
 
void sob() {
    sem_wait(SEM_K);

    printf(" sob %d spawned\n", d->n_sob % SOB_MAX);

    d->n_sob++;
    if (d->n_sob == SOB_MAX) {
        sem_set(SEM_DJED);
    }

    sem_set(SEM_K);
    sem_wait(SEM_SOB);
}

void pat() {
    sem_wait(SEM_K);

    printf(" pat %d spawned\n", d->n_pat % PAT_MAX);

    d->n_pat++;
    if (d->n_pat == PAT_MAX) {
        sem_set(SEM_DJED);
    }

    sem_set(SEM_K);
    sem_wait(SEM_PAT);
}

#define MAX_PROCESSES (500)
int pids[MAX_PROCESSES];
int p = 0;

void proc_create(void (*func)()) {
    if (p >= MAX_PROCESSES) return;

    int pid;
    while ((pid = fork()) < 0); // try until creation successful

    // child
    if (pid == 0) {
        (*func)();
        exit(0);

    // parent
    } else pids[p++] = pid;
}

void cleanup() {
    sem_destroy();
    mem_destroy();
    exit(0);
}

int main() {
    srand(time(NULL));

    if (!sem_create())
        return 1;

    if (!mem_create()) {
        sem_destroy();
        return 1;
    }

    d->n_sob = 0;
    d->n_pat = 0;

    sigset(SIGINT, cleanup);

    proc_create(djed);

    while (1) {
        sleep(rand() % 3 + 1);

        if (rand() % 2 && d->n_sob < SOB_MAX)
            proc_create(sob);

        if (rand() % 2)
            proc_create(pat);
    }
}