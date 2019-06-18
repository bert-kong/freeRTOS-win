#include <producer_consumer.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

static BaseType_t prod_task_id, cons_task_id;
static TaskHandle_t prod_handle = NULL;
static TaskHandle_t cons_handle = NULL;
static xQueueHandle m_queue_handle = NULL;

#define STACK_SIZE 128 * 2

static void producer_task(void* unused)
{
	printf("debug bxkong ---> %s::0x%08x\n", __func__, prod_task_id);
	BaseType_t ret;
	uint32_t data = 0;
	while (1)
	{

		ret = xQueueSendToBack(m_queue_handle, &data, 0);
		if (pdPASS == ret)
		{
			data += 1;
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
	uint32_t valid_id, data;
	BaseType_t ret;
	uint32_t number_of_packets = 0x1000000;
	uint32_t start = xTaskGetTickCount();


	valid_id = 0;
	
	while (1)
	{
		ret = xQueueReceive(m_queue_handle, &data, 0);

		if (ret == pdPASS)
		{
			if (valid_id != data)
			{
				printf("error 0x%x !!!\n", data);
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
	printf("number of paket 0x%x::0x%x::0x%08X\n", data, valid_id, elapsed);
}

void producer_consumer()
{
	m_queue_handle = xQueueCreate(0x1000, sizeof(uint32_t));

	prod_handle = xTaskCreate(producer_task,
		"producer",
		STACK_SIZE, /* stack size */
		NULL, /* data */
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

