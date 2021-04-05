// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 4
int sig[N] = {SIGUSR1, SIGUSR2, SIGTRAP, SIGCHLD};

int pid = 0;

void prekidna_rutina(int sig) {
    kill(pid, SIGKILL);
    exit(0);
}

int main(int argc, char* argv[]) {
    pid = atoi(argv[1]);
    sigset(SIGINT, prekidna_rutina);

    srand(time(NULL));

    while (1) {
        /* odspavaj 3-5 sekundi */
        sleep(rand() % 3 + 3);

        /* slučajno odaberi jedan signal (od 4) */
        /* pošalji odabrani signal procesu 'pid' funkcijom kill*/
        kill(pid, sig[rand() % N]);
    }
}