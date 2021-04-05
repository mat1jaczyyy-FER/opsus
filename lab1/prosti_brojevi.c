// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int pauza = 0;
int broj = 1000000001;
int zadnji = 1000000001;

void periodicki_ispis() {
    printf("zadnji prosti broj = %d\n", zadnji);
}

void postavi_pauzu() {
    pauza = 1 - pauza;
}

void prekini() {
    periodicki_ispis();
    exit(0);
}

int prost(int n) {
	int i, max;

	if (!(n & 1)) /* je li paran? */
		return 0;

	max = sqrt(n);
	for (i = 3; i <= max; i += 2)
		if (!(n % i))
			return 0;

	return 1; /* broj je prost! */
}

int main() {
    sigset(SIGALRM, periodicki_ispis);
    sigset(SIGINT, postavi_pauzu);
    sigset(SIGTERM, prekini);

    struct itimerval t;
    t.it_value.tv_sec = 5;
    t.it_value.tv_usec = 0;
    t.it_interval.tv_sec = 5;
    t.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &t, NULL);

    while (1) {
        if (prost(broj))
            zadnji = broj;
        broj++;
        while (pauza)
            pause();
    }
}