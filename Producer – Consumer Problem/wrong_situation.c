#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define ITER 20

void* thread_p(void* arg);
void* thread_c(void* arg);

int x;

int main() {
    pthread_t tid1, tid2;

    pthread_create(&tid1, NULL, thread_p, NULL);
    pthread_create(&tid2, NULL, thread_c, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    if (x != 0)
        printf("BOOM! counter=%d\n", x);
    else
        printf("OK counter=%d\n", x);

    return 0;
}

void* thread_p(void* arg) {//producer
    int i, val;
    for (i = 0; i < ITER; i++) {
        val = x;
        printf("Poducuer %u:%d\n", (unsigned int)pthread_self(), x);
        x = val + 1;
    }

    pthread_exit(NULL);
}

void* thread_c(void* arg) {//consumer
    int i, val;
    for (i = 0; i < ITER; i++) {
        val = x;
        printf("Consumer %u: %d\n", (unsigned int)pthread_self(), val);
        x = val - 1;
    }

    pthread_exit(NULL);
}