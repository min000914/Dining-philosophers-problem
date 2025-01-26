#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define ITER 30//반복횟수
#define MAX 10//BufferSize

void* thread_p(void* arg);
void* thread_c(void* arg);

int x = 0;
sem_t m;//전역변수 x에 대한 상호배제를 위한 Sem
sem_t fill;//Buffer가 비었을 경우 소비자의 소비를 막기위한 CV
sem_t empty;//Buffer가 꽉 찼을 경우 생산자의 생산을 막기위한 CV

int main() {
    pthread_t tid1, tid2;
    sem_init(&m, 0, 1);
    sem_init(&fill, 0, 0);
    sem_init(&empty, 0, MAX);
    pthread_create(&tid1, NULL, thread_p, NULL);
    pthread_create(&tid2, NULL, thread_c, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    if (x != 0)
        printf("BOOM! counter=%d\n", x);
    else
        printf("OK counter=%d\n", x);

    sem_destroy(&m);
    sem_destroy(&fill);
    sem_destroy(&empty);

    return 0;
}

void* thread_p(void* arg) {//생산자
    int i, val;
    for (i = 0; i < ITER; i++) {
        sem_wait(&empty);
        sem_wait(&m);

        val = x;
        printf("Producer %u:%d\n", (unsigned int)pthread_self(), x);
        x = val + 1;

        sem_post(&m);
        sem_post(&fill);
    }

    pthread_exit(NULL);
}

void* thread_c(void* arg) {
    int i, val;
    for (i = 0; i < ITER; i++) {
        sem_wait(&fill);
        sem_wait(&m);

        val = x;
        printf("Consumer %u: %d\n", (unsigned int)pthread_self(), val);
        x = val - 1;

        sem_post(&m);
        sem_post(&empty);
    }

    pthread_exit(NULL);
}