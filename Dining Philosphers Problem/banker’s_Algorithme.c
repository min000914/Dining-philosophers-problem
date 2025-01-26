#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM 5
#define ITER 100 // 한 철학자 당 Loop 횟수

sem_t forks[NUM]; // forks

int maximum[NUM][NUM] = { //각 철학자가 원하는 포크의 수
    {1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1},
    {1, 0, 0, 0, 1}};
int allocated[NUM][NUM] = {0}; //각 철학자에게 할당된 포크의 수
int available[NUM] = {1, 1, 1, 1, 1}; //각 포크가 얼마나 남아있는지 확인
sem_t x; //전역변수인 allocated와 available변경 시 상호배제를 위한 sem
pthread_mutex_t mutex;//만약 UnSafe상태라면, 해당 Thread를 Block시키고
pthread_cond_t cond;//Safe상태인 Thread의 수행을 마치면 BroadCast를 통해 Block된 모든 Thread를 UnLock한다.

void pickup(int philosopher_num, int fork)
{
    sem_wait(&forks[fork]);
    sem_wait(&x);//전역변수에 대한 상호배제
    allocated[philosopher_num][fork] = 1;//fork PickUp시 본인에게 할당된 fork수를 늘린다.
    available[fork] = 0;//남은 fork수는 줄인다.
    sem_post(&x);
}

void putdown(int philosopher_num, int fork)
{
    sem_post(&forks[fork]);
    sem_wait(&x);//전역변수에 대한 상호배제
    allocated[philosopher_num][fork] = 0;//fork putdown시 본인에게 할당된 fork수를 0으로 한다.
    available[fork] = 1;//남은 fork수는 늘인다.
    sem_post(&x);
}

void thinking(int philosopher_num)
{
    printf("Philosopher %d is thinking.\n", philosopher_num);
    return;
}

void eating(int philosopher_num)
{
    printf("Philosopher %d is eating.\n", philosopher_num);
    return;
}

int safe(int philosopher_num)
{
    int need[NUM];//해당 철학자가 필요한 fork 수 를 저장
    int safe = 1;
    sem_wait(&x);//전역변수에 대한 상호배제
    for (int i = 0; i < NUM; i++)
    {
        //지금 필요한 fork수 = 최대 필요한fork수 - 현재 가지고 있는 fork수
        need[i] = maximum[philosopher_num][i] - allocated[philosopher_num][i];
    }
    for (int i = 0; i < NUM; i++)
    {
        if (available[i] < need[i])//가용가능한 fork보다 많이 필요로하면 UnSafe상태이다.
        {
            safe = 0;
            break;
        }
    }
    sem_post(&x);
    return safe;
}
void *philosopher(void *arg)
{
    int philosopher_num = (unsigned long int)arg;
    int left_fork = philosopher_num;
    int right_fork = (philosopher_num + 1) % NUM;

    for (int i = 0; i < ITER; i++)
    {
        pthread_mutex_lock(&mutex);
        while (!safe(philosopher_num))
        {
            pthread_cond_wait(&cond, &mutex);//UnSafe한 상태라면 자러간다.
        }
        pthread_mutex_unlock(&mutex);

        pickup(philosopher_num, left_fork);
        printf("Philosopher %d picks up the fork %d.\n", philosopher_num, left_fork);
        pickup(philosopher_num, right_fork);
        printf("Philosopher %d picks up the fork %d.\n", philosopher_num, right_fork);

        eating(philosopher_num);

        putdown(philosopher_num, right_fork);
        printf("Philosopher %d puts down the fork %d.\n", philosopher_num, right_fork);
        putdown(philosopher_num, left_fork);
        printf("Philosopher %d puts down the fork %d.\n", philosopher_num, left_fork);

        thinking(philosopher_num);

        pthread_cond_broadcast(&cond);//Safe상태가 되면, 자고 있는 모든 Thread를 깨운다.
    }
    return NULL;
}

int main()
{
    clock_t start, end;//시간측정
    double cpu_time_used;//시간측정

    pthread_t threads[NUM];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    for (int i = 0; i < NUM; i++)
    {
        sem_init(&forks[i], 0, 1);
    }
    sem_init(&x, 0, 1);

    start = clock(); //시간측정
    for (unsigned long int i = 0; i < NUM; i++)
    {
        pthread_create(&threads[i], NULL, philosopher, (void *)i);
    }

    for (int i = 0; i < NUM; i++)
    {
        pthread_join(threads[i], NULL);
    }

    end = clock(); // 종료 시간 측정
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC; // 수행 시간 계산

    printf("Total execution time: %f seconds\n", cpu_time_used);

    return 0;
}
