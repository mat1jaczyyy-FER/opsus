// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N (5)
char f[N], s[N];

void print_state(int i) {
    for (int k = 0; k < N; k++)
        printf("%c ", f[k]);
    
    printf("(%d)\n", i + 1);
}

pthread_mutex_t mutex;
pthread_cond_t cond[N];

void enter_critical() { pthread_mutex_lock(&mutex); }
void leave_critical() { pthread_mutex_unlock(&mutex); }

void misliti() {
    sleep(3);
}

void jesti(int i) {
    int h = (i + N - 1) % N;
    int j = (i + 1) % N;

    enter_critical();
    
        f[i] = 'o';

        while (!s[i] || !s[j])
            pthread_cond_wait(&cond[i], &mutex);

        s[i] = 0;
        s[j] = 0;
        f[i] = 'X';

        print_state(i);
    
    leave_critical();

    sleep(2); // njam_njam

    enter_critical();
    
        f[i] = 'O';

        s[i] = 1;
        s[j] = 1;

        pthread_cond_signal(&cond[h]);
        pthread_cond_signal(&cond[j]);

        print_state(i);
    
    leave_critical();
}

void* process(void* i) {
    while (1) {
        misliti();
        jesti(*(int*)i);
    }
}

int main() {
    memset(f, '0', N);
    memset(s, 1, N);

    pthread_mutex_init(&mutex, NULL);

    pthread_t threads[N];
    int i[N];

    for (int k = 0; k < N; k++) {
        pthread_cond_init(cond + k, NULL);
        i[k] = k;

        while (pthread_create(threads + k, NULL, process, i + k));
    }

    for (int k = 0; k < N; k++)
        pthread_join(threads[k], NULL);

    return 0;
}