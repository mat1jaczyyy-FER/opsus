// Dominik Matijaca 0036524568
#pragma GCC diagnostic ignored "-Wimplicit-function-declaration"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int A = 0;
int M;

void* process() {
    for (int i = 0; i < M; i++)
        A++;
}

int main(int argc, char* argv[]) {
    int N = atoi(argv[1]);
    M = atoi(argv[2]);

    pthread_t threads[M];

    for (int i = 0; i < N;)
        if (!pthread_create(threads + i, NULL, process, NULL))
            i++;

    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);
    
    printf("A=%d\n", A);
    return 0;
}