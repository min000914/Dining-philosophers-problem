#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM 5

pthread_mutex_t forks[NUM]; // forks


void pickup(int philosopher_num)
{
    pthread_mutex_lock(&forks[philosopher_num % NUM]);
}

void putdown(int philosopher_num)
{
    if (pthread_mutex_trylock(&forks[philosopher_num % NUM]) == 0) {
        printf("puts down the fork %d.\n", i);
        pthread_mutex_unlock(&forks[philosopher_num % NUM]);
    }
}

void thinking(int philosopher_num)
{
    printf("Philosopher %d is thinking\n", philosopher_num);
    //usleep(rand() % 500000); // 임의의 대기 시간 추가
}

void eating(int philosopher_num)
{
    printf("Philosopher %d is eating\n", philosopher_num);
    //usleep(rand() % 500000); // 임의의 대기 시간 추가
}

void *philosopher(void *arg)
{
    int philosopher_num = (unsigned long int)arg;
    int left_fork = philosopher_num;
    int right_fork = (philosopher_num + 1) % NUM;

    while (1)
    {
        //sem_wait(&DeadLockChecking);
        pickup(left_fork);
        printf("philosopher %d picks up the fork %d.\n", philosopher_num, left_fork);
        pickup(right_fork);
        printf("philosopher %d picks up the fork %d.\n", philosopher_num, right_fork);
        eating(philosopher_num);
        putdown(right_fork);
        printf("philosopher %d puts down the fork %d.\n", philosopher_num, right_fork);
        putdown(left_fork);
        printf("philosopher %d puts down the fork %d.\n", philosopher_num, left_fork);

        thinking(philosopher_num);
        //sem_post(&DeadLockChecking);
    }
    return NULL;
}

void *dlc_thread(void *arg)//DeadLock을 검출하는 Thread
{
    int deadlock_counter = 0;//모두가 포크를 들고 있는 상황이 무조건 DeadLock인 것은 아니니
    //그렇기 때문에 DeadLock검출이 3번이상됐다면 DeadLock으로 판단하도록한다.
    while (1)
    {
        sleep(2); // 2초마다 Deadlock을 체크

        // 모든 포크에 대해 시도해보고 획득할 수 없는 경우 Deadlock으로 판단
        bool deadlock = true;
        for (int i = 0; i < NUM; i++)
        {
            while (pthread_mutex_trylock(&forks[i]) == 0)//모든 fork의 mutex를 Lock해보고
            {
                pthread_mutex_unlock(&forks[i]);//하나라도 Lock이 걸렸다면
                deadlock = false;//DeadLock상태가 아니다.
                break;
            }
        }

        if (deadlock)
        {
            deadlock_counter++;
            printf("Deadlock detected! Attempt #%d\n", deadlock_counter);
            if (deadlock_counter >= 3)//DeadLock이 3번 검출됐다면
            {
                printf("Deadlock recovery in progress.\n");

                for (int i = 0; i < 5; i++)
                {//모든 fork를 내려놓도록 한다.
                    putdown(i);
                }

                printf("Deadlock recovery completed.\n");
                deadlock_counter = 0;
            }
        }
        else
        {
            deadlock_counter = 0;
        }
    }
}

int main()
{
    pthread_t threads[NUM];
    pthread_t dlc;

    for (int i = 0; i < NUM; i++)
    {
        pthread_mutex_init(&forks[i], NULL);
    }
    for (unsigned long int i = 0; i < NUM; i++)
    {
        pthread_create(&threads[i], NULL, philosopher, (void *)i);
    }

    pthread_create(&dlc, NULL, dlc_thread, NULL);

    for (int i = 0; i < NUM; i++)
    {
        pthread_join(threads[i], NULL);
    }

    pthread_join(dlc, NULL);

    for (int i = 0; i < NUM; i++)
    {
        pthread_mutex_destroy(&forks[i]);
    }
    printf("NO DEADLOCK\n");
    return 0;
}
