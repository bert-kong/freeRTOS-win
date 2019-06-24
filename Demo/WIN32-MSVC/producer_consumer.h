#ifndef __PRODUCER_CONSUMER_H__
#define __PRODUCER_CONSUMER_H__


#include "FreeRTOS.h"
#include "task.h"

typedef struct bkMessage_t* bkMessage;

void producer_consumer();

#if 0
struct _ProducerConsumerStruct
{
	TaskHandle_t handle;
	xQueueHandle queue;
}
#endif


//template<typename T>
#endif // !__PRODUCER_CONSUMER_H__