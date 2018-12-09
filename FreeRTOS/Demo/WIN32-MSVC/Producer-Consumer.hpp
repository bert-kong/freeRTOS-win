
#ifndef __PRODUCER_CONSUMER_H__
#define __PRODUCER_CONSUMER_H__

#include<cstdio>
#include<cstdint>
#include "FreeRTOS.h"
#include "semphr.h"

template<typename T>
class Producer
{

public:
    static void run(void *data);

public:
    explicit
    Producer(QueueHandle_t queue)
        :m_queue(queue)
    {
    }

    const bool write(const T *);

private:
    Producer();
    Producer(const Producer &);
    const Producer &operator=(const Producer &);

private:
    QueueHandle_t m_queue; 

};


template<typename T>
class Consumer
{
public:
    static void run(void *data);

public:
    explicit
    Consumer(QueueHandle_t queue)
        :m_queue(queue)
    {
    }

    const bool read(T *obj);

private:
    Consumer();
    Consumer(const Consumer &);
    const Consumer &operator=(const Consumer &);

private:
    QueueHandle_t m_queue; 
};


void main_producer_consumer();

#endif
