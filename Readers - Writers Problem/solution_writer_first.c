#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define ITER 15//반복횟수
#define R_NUM 3//Reader수
#define W_NUM 3
void *thread_writer(void *arg);
void *thread_reader(void *arg);

int readThing = 0;//Writer가 변경하고 Reader 읽을 변수
sem_t x;//Reader에서 변경되는 readCount를 상호배제하기 위한 Sem
sem_t y;//Writer에서 변경되는 WriteCount를 상호배제하기 위한 Sem
sem_t z;//후술.
sem_t rsem;//대기 중이던 Writer가 모두 수행을 끝낼 때까지 Reader의 접근을 막는 Sem(WirterPreference)
sem_t wsem;//Writer 혹은 Reader가 수행중일 때 다른Writer의 접근을 막기위한 CV를 위한 Sem
int readCount = 0;//현재 Reading중인 Reader수
int writeCount = 0;//현재 Write실행을 위해 대기중인 Writer수


int main()
{
    pthread_t write_tid[W_NUM];
    pthread_t reader_tid[R_NUM];
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);
    sem_init(&z, 0, 1);
    sem_init(&rsem, 0, 1);
    sem_init(&wsem, 0, 1);

    for (unsigned int i = 0; i < R_NUM; i++)//3개의 Read Thread를 생성한다.
    {
        pthread_create(&reader_tid[i], NULL, thread_reader, (void*)(i+1));
    }
    for (unsigned int i = 0; i < R_NUM; i++)
    {
        pthread_create(&write_tid[i], NULL, thread_writer, (void*)(i+1));
    }
    for (unsigned int i = 0; i < R_NUM; i++)
    {
        pthread_join(reader_tid[i], NULL);
    }
    for (unsigned int i = 0; i < R_NUM; i++)
    {
        pthread_join(write_tid[i], NULL);
    }
    sem_destroy(&x);
    sem_destroy(&y);
    sem_destroy(&z);
    sem_destroy(&rsem);
    sem_destroy(&wsem);
    return 0;
}

void *thread_writer(void *arg)//Writer
{
    int i;
    int writeNum= (unsigned long int)arg;//자신이 몇번째 Write인지 저장
    for (i = 0; i < ITER; i++) {
        sem_wait(&y);//WriterCount를 위한 상호배제
        writeCount++;
        if (writeCount == 1)
            sem_wait(&rsem);//Writer에서 rsem을 잡고 있다면(대기중인 Wirter가 있다면) Read를 수행할 수 없게한다.
        sem_post(&y);
        sem_wait(&wsem);//다른 Writer가 수행중이거나, Read가 수행중이라면 Lock이걸린다.

        printf("%d WRITE%dth  %u:%d\n",writeNum, i, (unsigned int)pthread_self(), readThing++);

        sem_post(&wsem);
        sem_wait(&y);
        writeCount--;
        if (writeCount == 0)//Writer가 다 빠져 나간다면, Read가 접근할 수 있도록 rsem을 풀어준다.
            sem_post(&rsem);
        sem_post(&y);
    }

    pthread_exit(NULL);
}

void *thread_reader(void *arg)
{
    int i;
    int readerNum= (unsigned long int)arg;
    for (i = 0; i < ITER; i++)
    {
        sem_wait(&z);//z가 없다면, rsem을 잡기위해 Writer에서는 하나의 Writer만이 접근할 수 있지만
        //Reader에서는 다수의 Reader가 rsem을 잡기위해 경쟁할 수 있다. 이를 막아 WriterPreference를 준다.
        sem_wait(&rsem);
        sem_wait(&x);
        readCount++;
        if (readCount == 1)
            sem_wait(&wsem);
        sem_post(&x);
        sem_post(&rsem);
        sem_post(&z);

        printf("%d READ %dth  %u:%d\n", readerNum, i, (unsigned int)pthread_self(), readThing);

        sem_wait(&x);
        readCount--;
        if (readCount == 0)
            sem_post(&wsem);
        sem_post(&x);
    }

    pthread_exit(NULL);
}