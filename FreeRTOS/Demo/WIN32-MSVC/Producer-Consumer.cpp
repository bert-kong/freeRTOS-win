
#include "Producer-Consumer.hpp"
#include "task.h"

#define RUN_ITERATIONS 10000
static	TaskHandle_t prod_handle = NULL;
static	TaskHandle_t con_handle = NULL;

template<typename T>
const bool Producer<T>::write(const T *obj)
{
	return ::xQueueSend(m_queue, obj, TickType_t(0))==pdPASS;
}

template<typename T>
const bool Consumer<T>::read(T *obj)
{
	return ::xQueueReceive(m_queue, obj, TickType_t(0))==pdPASS;
}

class Message
{
public:
    Message(const int v=0)
        : value(v)
    {
        //::printf("debug ---> %s(const int)\n", __func__);
    }
    
    Message(const Message &o)
        :value(o.value)
    {        
        //::printf("debug ---> %s(const Message &)\n", __func__);

    }

    const Message& operator=(const Message &o)
    {
        //::printf("debug ---> %s(const Message &)\n", __func__);
        value = o.value;

        return *this;
    }

    const int getValue() const
    {
        return value;
    }

    void setValue(const int v) const
    {
        value = v;
    }

private:
    mutable int value;
};

template<typename T>
void Producer<T>::run(void *data)
{
    Producer<T> *prod = static_cast<Producer<T> *>(data);
    int value = 0;
    bool done = false;

    const T msg;

    while (!done)
    {
        msg.setValue(value);
        if (prod->write(&msg))
        {
            value += 1;
        }
		else 
		{
			::taskYIELD();
		}
        if (value > RUN_ITERATIONS)
        {
            done = true;
        }
    }

	TaskHandle_t handle = xTaskGetCurrentTaskHandle();
	printf("debug ---> Producer(%p) exit\n", handle);
	vTaskDelete(handle);
}

template<typename T>
void Consumer<T>::run(void *data)
{
    Consumer<T> *con = static_cast<Consumer<T> *>(data);
    bool done = false;
	int value = 0;;

    T msg;

    while (!done)
    {
        if (con->read(&msg))
        {
            ::printf("%d\n", msg.getValue());
			if (msg.getValue() != value)
			{
				::printf("debug ---> Warning error %d\n", msg.getValue());
				done = true;
			}
			value += 1;

            if (msg.getValue() >= RUN_ITERATIONS)
            {
                done = true;
            }
        }
    }

	TaskHandle_t handle = xTaskGetCurrentTaskHandle();
	printf("debug ---> Consumer %p exit\n", handle);
	vTaskDelete(handle);

}

void main_producer_consumer()
{
	QueueHandle_t queue = xQueueCreate(10, sizeof(Message));

	Producer<Message> prod(queue);
	Consumer<Message> con(queue);
	BaseType_t ret;

	ret = xTaskCreate(Producer<Message>::run,
					  "producer",
		              configMINIMAL_STACK_SIZE,
		              &prod,
		              tskIDLE_PRIORITY + 1,
		              &prod_handle);

	if (ret != pdPASS)
	{
		return;
	}

	if (xTaskCreate(Consumer<Message>::run, "consumer", configMINIMAL_STACK_SIZE, &con, tskIDLE_PRIORITY + 1, &con_handle) != pdPASS)
	{
		return;
	}

    vTaskStartScheduler();
	printf("debug ---> %s\n", __func__);

    for (;;)
    {
		printf("debug ---> loop\n");
    }
}
