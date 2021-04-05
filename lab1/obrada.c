// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <stdio.h>
#include <signal.h>

//#define DEBUG_PRINT

#define N 5    /* broj signala / razina prioriteta */
int sig[N] = {SIGUSR1, SIGUSR2, SIGTRAP, SIGCHLD, SIGINT};

int OZNAKA_CEKANJA[N];
int PRIORITET[N];
int TEKUCI_PRIORITET = -1;

void zabrani_prekidanje() {
    for (int i = 0; i < N; i++)
        sighold(sig[i]);
}

void dozvoli_prekidanje() {
    for (int i = 0; i < N; i++)
        sigrelse(sig[i]);
}

void print_debug() {
    #ifdef DEBUG_PRINT
        printf("      O_CEK[");
        
        for (int k = 0; k < N; k++)
            printf("%2d ", OZNAKA_CEKANJA[k]);

        printf("]  TEK_PRIOR=%2d  PRIOR[", TEKUCI_PRIORITET);
        
        for (int k = 0; k < N; k++)
            printf("%2d ", PRIORITET[k]);

        printf("]");
    #endif
}

void print_red(int i, const char* format, int j) {
    printf(" -");

    for (int k = 0; k < N; k++)
        printf(k == i? format: "  -", j + 1);

    print_debug();

    printf("\n");
}

void obrada_signala(int i) {
    /* obrada se simulira trošenjem vremena,
       obrada traje 5 sekundi, ispis treba biti svake sekunde */

    for (int j = 0; j < 5; j++) {
        sleep(1);

        if (j == 0)
            print_red(i, "  P", j);

        print_red(i, " %2d", j);

        if (j == 4)
            print_red(i, "  K", j);
    }
}

void prekidna_rutina(int s) {
    int n = -1;

    zabrani_prekidanje();

    printf(" -");

    for (int i = 0; i < N; i++) {
        printf(sig[i] == s? "  X" : "  -");
        if (sig[i] == s) n = i;
    }

    print_debug();

    printf("\n");
    
    if (n == -1) return;

    OZNAKA_CEKANJA[n] = 1;

    int x;
    do {
        /* odredi signal najveceg prioriteta koji ceka na obradu */
        x = -1;
        for (int j = TEKUCI_PRIORITET + 1; j < N; j++)
            if (OZNAKA_CEKANJA[j] != 0) x = j;
  
        /* ako postoji signal koji ceka i prioritetniji je od trenutnog posla, idi u obradu */
        if (x > -1) {
            OZNAKA_CEKANJA[x] = 0;
            PRIORITET[x] = TEKUCI_PRIORITET;
            TEKUCI_PRIORITET = x;
            dozvoli_prekidanje();
            obrada_signala(x);
            zabrani_prekidanje();
            TEKUCI_PRIORITET = PRIORITET[x];
        }
    } while (x > -1);

    dozvoli_prekidanje();
}

int main() {
    for (int i = 0; i < N; i++) {
        PRIORITET[i] = -1;
        sigset(sig[i], prekidna_rutina);
    }

    printf("Proces obrade prekida, PID=%d\nGP", getpid());

    for (int i = 0; i < N;)
        printf(" S%1d", ++i);

    printf("\n-----------------\n");

    for (int i = 0; i < 10;) {
        sleep(1);

        printf("%2d", ++i);

        for (int j = 0; j < N; j++)
            printf("  -");
        
        printf("\n");
    }

    printf("Zavrsio osnovni program\n");
    return 0;
}