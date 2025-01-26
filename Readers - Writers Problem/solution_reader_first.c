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
sem_t x; //Reader에서 변경되는 readCount를 상호배제하기 위한 Sem
sem_t wsem; //Writer 혹은 Reader가 수행중일 때 다른Writer의 접근을 막기위한 CV를 위한 Sem
int readCount = 0;//현재 Reading중인 Reader수

int main()
{
    pthread_t write_tid[W_NUM];
    pthread_t reader_tid[R_NUM];
    sem_init(&x, 0, 1);
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
    sem_destroy(&wsem);
    return 0;
}

void *thread_writer(void *arg) //Writer
{
    int i;
    int writeNum= (unsigned long int)arg;//자신이 몇번째 Write인지 저장
    for (i = 0; i < ITER; i++)
    {
        sem_wait(&wsem);//다른 Writer가 수행중이거나, Read가 수행중이라면 Lock이걸린다.

        printf("%d WRITE%dth  %u:%d\n",writeNum, i, (unsigned int)pthread_self(), readThing++);

        sem_post(&wsem);
    }

    pthread_exit(NULL);
}

void *thread_reader(void *arg)
{
    int i;
    int readerNum=(unsigned long int)arg;
    for (i = 0; i < ITER; i++)
    {
        sem_wait(&x);//ReadCount 상호배제
        readCount++;
        if (readCount == 1) //ReadCount가 1이라면,
            sem_wait(&wsem); //Writer가 wsem을 잡고있다면 멈추고 아니라면 Writer가 잡지 못하게 Lock을 건다.
        sem_post(&x);

        printf("%d READ %dth  %u:%d\n", readerNum, i, (unsigned int)pthread_self(), readThing);

        sem_wait(&x);
        readCount--;
        if (readCount == 0)//Read중인 모든 Reader가 빠져나갔다면
            sem_post(&wsem);//그 때 Writer의 Lock을 풀어준다.
        sem_post(&x);
    }

    pthread_exit(NULL);
}