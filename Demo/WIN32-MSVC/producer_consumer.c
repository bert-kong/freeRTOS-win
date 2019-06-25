#include <producer_consumer.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#define MAX_MSG_SENDS 0x1000

static QueueHandle_t queue;
static TaskHandle_t prod_handle;
static TaskHandle_t cons_handle;

const int stack_size = 128 * 2;

struct bkMessage_t
{
	uint32_t id;
	char name[20]; 
	uint32_t value;
};

void producer(void* data)
{
	struct bkMessage_t* msg = (struct bkMessage_t*) data;
	int done = 1;
	BaseType_t ret;

	while (done)
	{
		ret = xQueueSendToBack(queue, msg, 0);
		if (ret==pdPASS)
		{
			msg->id += 1;
			if (msg->id == MAX_MSG_SENDS)
			{
				done = 0;
			}
		}
		else if (ret == errQUEUE_FULL)
		{
			printf("queue is full\n");
			taskYIELD();
		}
	}
}

void consumer(void* data)
{
	int done = 1;
	struct bkMessage_t msg;
	BaseType_t ret;

	while (done)
	{
		ret = xQueueReceive(queue, &msg, 0);
		if (ret == pdPASS)
		{
			printf("debug ---> 0x%08X\n", msg.id);
			if (msg.id == MAX_MSG_SENDS - 1)
			{
				done = 0;
			}
		}
		else if (ret == errQUEUE_EMPTY)
		{
			printf("queue is empty\n");
			taskYIELD();
		}
	}
}

void producer_consumer()
{
	/* create queue */
	queue = xQueueCreate(16 * 2, sizeof(struct bkMessage_t));
	if (queue == NULL)
	{
	}
	
	/**
	 * Create tasks : producer and consumer
	 */

	/**
	 * struct bkMessage in this stack
	 */
	struct bkMessage_t msg;
	msg.id = 0;
	strcpy(msg.name, "producer");
	msg.value = 1000;

	BaseType_t ret;
	ret= xTaskCreate(producer, "producer", stack_size, &msg, tskIDLE_PRIORITY+2, &prod_handle);
	if (ret != pdPASS)
	{
	}

	ret = xTaskCreate(consumer, "consumer", stack_size, NULL, tskIDLE_PRIORITY+2, &cons_handle);
	if (ret != pdPASS)
	{
	}

	/* start scheduler */
	vTaskStartScheduler();
}
