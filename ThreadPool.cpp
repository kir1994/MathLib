#include "ThreadPool.h"
#include "math.h"

void* pool_func(void *arg)
{
    ThreadPool *tp = (ThreadPool *)arg;
    sem_wait(&(tp->c));  
    while(!(tp->flag))
    {          
        pthread_mutex_lock(&tp->qmutex);
        Longnum num = tp->task.front();
        tp->task.pop();
        pthread_mutex_unlock(&tp->qmutex);
        Longnum d = gcd(num, tp->num);
        if(d != 1)
        {
          pthread_mutex_lock(&(tp->end_mutex));
          if(tp->flag)
          {
            pthread_mutex_unlock(&(tp->end_mutex));
            pthread_exit(EXIT_SUCCESS);
          }
          tp->res = d;
          tp->flag = true;   
          pthread_mutex_unlock(&(tp->end_mutex));
          pthread_exit(EXIT_SUCCESS);
        }        
    }    
    pthread_exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
ThreadPool::ThreadPool(int thread_num): size(thread_num) 
{
    if (thread_num <= 0)         
        cout << "Illegal argument given" << endl;
    else
    {        
        flag = false;
        cond = false;
        sem_init(&c, 0, 0);
        pthread_mutex_init(&end_mutex, nullptr);
        pthread_mutex_init(&qmutex, nullptr);
        pos = new pthread_t[size];
        for (int i = 0; i < size; i++)
        {
            pthread_create(&(pos[i]), nullptr, pool_func, this);
        }       
    }
}
ThreadPool::~ThreadPool() 
{
    if(pos != nullptr)
    {
        for(int i = 0; i < size; i++)
          pthread_join((this->pos[i]), nullptr);
        delete[] pos;
    }
    sem_destroy(&c);
    pthread_mutex_destroy(&end_mutex);
    pthread_mutex_destroy(&qmutex);
}

bool ThreadPool::acquire(Longnum& new_task)
{
    if(!flag)
    {
      task.push(new_task);
      sem_post(&c);
      return true;
    }else
      return false;    
}
