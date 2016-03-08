
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include "longnums.h"
#include <semaphore.h>
#include <queue>

using namespace std;

const unsigned int DEFAULT_SIZE = 4;

class ThreadPool
{
public:   
    ThreadPool(int thread_num = DEFAULT_SIZE);
    ~ThreadPool();
    bool acquire(Longnum&);
    queue<Longnum> task;
    Longnum num;
    Longnum res;
    sem_t c;
    pthread_mutex_t end_mutex;
    pthread_mutex_t qmutex;
    volatile bool flag;
    bool cond;
protected: 
    pthread_t* pos;
    int size;
};

#endif