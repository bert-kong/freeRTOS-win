#include <stdio.h>
#include <string>
#include <stdexcept>
#include "producer-consumer.hpp"
#include "bk-List.hpp"

#define MAX_NUMBER_PACKAGE 1000
#define __BLOCK_TASK__

#define QUEUE_SIZE 100
static TaskHandle_t prod, cons;
static float max_value = 1000;

const int bk::ProducerConsumer<QUEUE_SIZE>::stack_size = 128;
const int bk::ProducerConsumer<QUEUE_SIZE>::priority = tskIDLE_PRIORITY + 1;;

/**
 *   Implementation of ProducerConsumer
 */
template<uint8_t N>
bk::ProducerConsumer<N>::ProducerConsumer()
	:m_queue(xQueueCreate(N, sizeof(bk::ProducerConsumer<QUEUE_SIZE>::BankAccount))),
	 m_done(false), m_id(0), m_balance(0), m_previous_value(0)
{
	if (m_queue==NULL)
	{
		throw std::invalid_argument("failed to create queue");
		::printf("debug ---> queue create failed\n");
	}
}

template<uint8_t N>
void bk::ProducerConsumer<N>::generateData()
{
}

template<uint8_t N>
void bk::ProducerConsumer<N>::onSendToQueue()
{
	char buf[32];

	::sprintf(buf, "balance %d", m_balance);
	bk::ProducerConsumer<N>::BankAccount account(m_id, m_balance, buf);

	BaseType_t ret;
#ifdef __BLOCK_TASK__
	ret = xQueueSendToBack(m_queue, &account, portMAX_DELAY);
	//::printf("debug ---> %s::%d\n", __func__, ret);
#else
	while (true)
	{
		ret = xQueueSendToBack(m_queue, &account, 0);

		if (ret==errQUEUE_FULL)
		{
			::taskYIELD();
		}
		else
		{
			/* successfull */
			break;
		}
	}
#endif
	
	m_id += 1;
	m_balance += 10;
	if (m_id == MAX_NUMBER_PACKAGE)
	{
		m_done = true;
	}
}

template<uint8_t N>
void bk::ProducerConsumer<N>::processData()
{
	BaseType_t ret;
	bk::ProducerConsumer<N>::BankAccount account(0, 0, "name");

#ifdef __BLOCK_TASK__
	ret = ::xQueueReceive(m_queue, &account, portMAX_DELAY);
#else
	while (pdPASS!=::xQueueReceive(m_queue, &account, 0))
	{
		::taskYIELD();
	}
#endif

	if (account.id == m_previous_value)
	{
		::printf("Id = %d, %s\n", account.id, account.getName());
		m_previous_value = account.id + 1;
	}
	else
	{
		::printf("-------------> error %d <--------------\n", account.id);
		m_done = true;
	}
}


/**
 *  Static Functions
 */
void bk::ProducerConsumer<QUEUE_SIZE>::producer(void* data)
{
	uint32_t start = ::xTaskGetTickCount();
	bk::ProducerConsumer<QUEUE_SIZE>* prod = reinterpret_cast<bk::ProducerConsumer<QUEUE_SIZE> *>(data);

	while (!prod->done())
	{
		prod->onSendToQueue();
	}


	::printf("debug ---> %s::0x%08X\n", __func__, ::xTaskGetTickCount()-start);
}

void bk::ProducerConsumer<QUEUE_SIZE>::consumer(void* data)
{
	bk::ProducerConsumer<QUEUE_SIZE>* cons = reinterpret_cast<bk::ProducerConsumer<QUEUE_SIZE> *>(data);

	while (true)
	{
		if (cons->done() && cons->isQueueEmpty())
		{
			break;
		}
		cons->processData();
	}

	::printf("debug ---> %s\n", __func__);
}

void test_bk_list()
{
	bk::List pxDelayedTaskList;
	bk::task_control_block_t current_tcb;
	bk::task_control_block_t current_tcb2;

	bk::List::list_item_t item;

	pxDelayedTaskList.insert(&current_tcb.xEventListItem);
	pxDelayedTaskList.insert(&current_tcb2.xEventListItem);
}

void main_producer_consumer()
{
	test_bk_list();
}