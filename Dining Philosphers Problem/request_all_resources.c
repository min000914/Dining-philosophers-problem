#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

#define NUM 5 // 철학자 수
#define ITER 100 // 한 철학자 당 Loop 횟수

pthread_mutex_t forks[NUM]; // forks
sem_t once; // 한번에 필요한 모든 fork를 얻기 전까지 Scheduling을 막는다.

void pickup(int philosopher_num)
{
    pthread_mutex_lock(&forks[philosopher_num % NUM]);
}

void putdown(int philosopher_num)
{
    pthread_mutex_unlock(&forks[philosopher_num % NUM]);
}

void thinking(int philosopher_num)
{
    printf("Philosopher %d is thinking\n", philosopher_num);
}

void eating(int philosopher_num)
{
    printf("Philosopher %d is eating\n", philosopher_num);
}

void *philosopher(void *arg)
{
    int philosopher_num = (unsigned long int)arg;
    int left_fork = philosopher_num;
    int right_fork = (philosopher_num + 1) % NUM;

    for (int i = 0; i < ITER; i++) {
        sem_wait(&once); // 필요한 모든 fork를 얻기 전까지 다른 철학자가 접근하지 못하도록한다.
        pickup(left_fork);
        printf("Philosopher %d picks up the fork %d.\n", philosopher_num, left_fork);
        pickup(right_fork);
        printf("Philosopher %d picks up the fork %d.\n", philosopher_num, right_fork);
        sem_post(&once); // 필요한 모든 fork를 얻었다면, once를 풀어준다.

        eating(philosopher_num);

        putdown(right_fork);
        printf("Philosopher %d puts down the fork %d.\n", philosopher_num, right_fork);
        putdown(left_fork);
        printf("Philosopher %d puts down the fork %d.\n", philosopher_num, left_fork);

        thinking(philosopher_num);
    }
    return NULL;
}


int main()
{
    pthread_t threads[NUM];
    clock_t start, end;
    double cpu_time_used;

    for (int i = 0; i < NUM; i++) {
        pthread_mutex_init(&forks[i], NULL);
    }
    sem_init(&once, 0, 1);

    start = clock(); // 시작 시간 측정

    for (unsigned long int i = 0; i < NUM; i++) {
        pthread_create(&threads[i], NULL, philosopher, (void *)i);
    }

    for (int i = 0; i < NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    end = clock(); // 종료 시간 측정
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // 수행 시간 계산

    printf("Total execution time: %f seconds\n", cpu_time_used);

    for (int i = 0; i < NUM; i++) {
        pthread_mutex_destroy(&forks[i]);
    }
    sem_destroy(&once);
    return 0;
}
