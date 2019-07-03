#include "producer-consumer.hpp"
#include <stdio.h>
#include <string>

#include "task.h"
#define Q_CAPACITY 2
static TaskHandle_t prod, cons;
static float max_value = 1000;
static bk::Queue<bk::BankAccount<int>, Q_CAPACITY> *queue=NULL;

std::string enum2string(const int state)
{
	switch (state)
	{
	case eRunning:
		return  std::string("running");
	case eReady:
		return std::string("ready");
	case eBlocked:
		return std::string("blocked");
	case eSuspended:
		return std::string("suspended");
	case eDeleted:
		return std::string("deleted");
	case eInvalid:
		return std::string("invalid");
	default:
		return std::string("eInvalid");
	}

	return std::string("");
}

void producer(void* data)
{
	bk::BankAccount<int>* acnt = static_cast<bk::BankAccount<int> *>(data);
	int done = 0;
	int value = 0;
	TaskStatus_t task;

	while (!done)
	{
		/* queue tasks state */
		vTaskGetInfo(prod, &task, pdTRUE, eInvalid);
		printf("producer task current state ---> %s\n", enum2string(task.eCurrentState).c_str());
		printf("producer task current priority ---> %d\n", task.uxCurrentPriority);

		vTaskGetInfo(cons, &task, pdTRUE, eInvalid);
		printf("consumer task current state ---> %s::%d\n", enum2string(task.eCurrentState).c_str(), task.eCurrentState);
		printf("consumer task current priority ---> %d\n", task.uxCurrentPriority);

		/* if return !=0, yield */
		if (queue->push_back(*acnt))
		{
			//printf("producer yield\n");
			taskYIELD();
		}
		else
		{
			value += 1;
			acnt->deposit(value);
			if (value > max_value)
			{
				done = 1;
			}
		}
	}
	printf("debug ---> producer\n");
}

void consumer(void* data)
{
	bk::BankAccount<int> acnt;

	int done = 0;
	while (!done)
	{
		if (queue->pop_front(&acnt))
		{
			// empty
			//printf("consumer yield\n");
			taskYIELD();
		}
		else
		{
			printf("consumer ---> %d\n", acnt.balance());
			if (acnt.balance()==max_value-1)
			{
				done = 1;
			}
		}
	}
	printf("debug ---> consumer\n");
}


void test_queue()
{
	bk::Queue<bk::BankAccount<float>> queue;

	for (int i = 0; i < 10; i++)
	{
		queue.push_back(bk::BankAccount<float>(1000.0 * (i + 1)));
	}
}

void producer_consumer()
{
	/**
	 * struct { func, name, stack size, parameter, priority, *handle) 
	 */

	BaseType_t ret;
	queue = new bk::Queue<bk::BankAccount<int>, Q_CAPACITY>();
	if (queue==NULL)
	{
		return;
	}


	bk::BankAccount<int> acct;
	ret = xTaskCreate(producer,   /* task */
		              "producer", /* name */
		              128,        /* stack */
		              &acct,      /* data */
		              tskIDLE_PRIORITY + 1,  /* priority */
		              &prod /* task handle */ );

	if (ret == pdPASS)
	{
		printf("pdPASS task producer 0x%x::0x%x\n", ret, prod);
	}
	else
	{
		printf("pdFAIL task consumer 0x%x::0x%x\n", ret, cons);
		return;
	}

	ret = xTaskCreate(consumer, "consumer", 128, NULL, tskIDLE_PRIORITY + 1, &cons);
	if (ret == pdPASS)
	{
		printf("pd PASS task consumer 0x%x::0x%x\n", ret, cons);
	}
	else
	{
		printf("pdFAIL task consumer 0x%x::0x%x\n", ret, cons);
		vTaskDelete(prod);
		return;
	}

	vTaskStartScheduler();

	for (;;)
	{
	}
}