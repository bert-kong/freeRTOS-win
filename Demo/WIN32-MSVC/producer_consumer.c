#include <producer_consumer.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

static BaseType_t prod_task_id, cons_task_id;
static TaskHandle_t prod_handle = NULL;
static TaskHandle_t cons_handle = NULL;
static xQueueHandle m_queue_handle = NULL;

struct bkMessage_t
{
	uint32_t id;
	char name[20]; 
	uint32_t value;
};

#define STACK_SIZE 128 * 4


static void producer_task(void* data)
{
	printf("debug bxkong ---> %s::0x%08x\n", __func__, prod_task_id);
	BaseType_t ret;

	bkMessage msg = (bkMessage)(data);
	msg->value = 0x20;

	while (1)
	{
		ret = xQueueSendToBack(m_queue_handle, msg, 0);
		if (pdPASS == ret)
		{
			msg->id += 1;
		}
		else 
		{
			taskYIELD();
		}
	}
}

static void consumer_task(void* unused)
{
	printf("debug bxkong ---> %s\n", __func__);
	uint32_t valid_id;
	BaseType_t ret;
	uint32_t number_of_packets = 0x100;
	uint32_t start = xTaskGetTickCount();

	bkMessage msg = malloc(sizeof(struct bkMessage_t));
	
	valid_id = 0;
	
	while (1)
	{
		ret = xQueueReceive(m_queue_handle, msg, 0);

		if (ret == pdPASS)
		{
			if (valid_id != msg->id)
			{
				printf("error 0x%x::0x%x\n", valid_id, msg->id);
				break;
			}
			if (valid_id == number_of_packets)
			{
				break;
			}
			valid_id += 1;
		}
		else
		{
			taskYIELD();
		}
	}

	uint32_t elapsed = xTaskGetTickCount() - start;
	printf("number of paket 0x%x::0x%x::0x%08X\n", msg->id, valid_id, elapsed);

	free(msg);
}

void producer_consumer()
{
	printf("message size = %d\n", sizeof(struct bkMessage_t));
	m_queue_handle = xQueueCreate(0x100, sizeof(struct bkMessage_t));

	bkMessage msg = malloc(sizeof(struct bkMessage_t));
	msg->id = 0;
	strcpy(msg->name, "free RTOS task");

	prod_handle = xTaskCreate(producer_task,
		"producer",
		STACK_SIZE, /* stack size */
		msg, /* data */
		tskIDLE_PRIORITY + 1, /* priority */
		&prod_handle);

	cons_handle = xTaskCreate(consumer_task,
		"consumer",
		STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY + 1,
		&cons_handle);

	vTaskStartScheduler();

}

