
#include "Producer-Consumer.hpp"
#include "task.h"

#define RUN_ITERATIONS 1000000

template<typename T, uint8_t N>
const bool CircularBuffer<T, N>::put(const T &obj)
{
    bool ret = false;

    while (::xSemaphoreTake(m_lock, (TickType_t)0)==pdTRUE)
    {
		if (isFull())
		{
			::xSemaphoreGive(m_lock);
			::taskYIELD();
		}
		else
		{
            m_buffer[m_wr] = obj;
            m_wr = (m_wr + 1U) % m_size;
			::xSemaphoreGive(m_lock);

            ret = true;
			break;
        }
    }

    return ret;
}

template<typename T, uint8_t N>
const bool CircularBuffer<T, N>::get(T &obj)
{
    bool ret = false;

    while (::xSemaphoreTake(m_lock, (TickType_t)0) == pdTRUE)
    {
		if (isEmpty())
		{
			::xSemaphoreGive(m_lock);
			::taskYIELD();
		}
		else
        {
            obj = m_buffer[m_rd];
            m_rd = (m_rd + 1U) % m_size;
			::xSemaphoreGive(m_lock);
            ret = true;
			break;
        }
    }

    return ret;
}

template<typename T, uint8_t N>
const bool Producer<T, N>::write(const T &obj)
{
	return m_queue.put(obj);
}

template<typename T, uint8_t N>
const bool Consumer<T, N>::read(T &obj)
{
    return m_queue.get(obj);
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

template<typename T, uint8_t N>
void Producer<T, N>::run(void *data)
{
    Producer<T, N> *prod = static_cast<Producer<T, N> *>(data);
    int value = 0;
    bool done = false;

    const T msg;

    while (!done)
    {
        msg.setValue(value);
        if (prod->write(msg))
        {
            value += 1;
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

template<typename T, uint8_t N>
void Consumer<T, N>::run(void *data)
{
    Consumer<T, N> *con = static_cast<Consumer<T, N> *>(data);
    bool done = false;
	int value = 0;;

    T msg;

    while (!done)
    {
        if (con->read(msg))
        {
            ::printf("%d\n", msg.getValue());
			if (msg.getValue() != value)
			{
				::printf("debug ---> %d\n", msg.getValue());
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
	CircularBuffer<Message, 2> queue;
	Producer<Message, 2> prod(queue);
	Consumer<Message, 2> con(queue);
	BaseType_t ret;
	TaskHandle_t prod_handle;
	TaskHandle_t con_handle;

	ret = xTaskCreate(Producer<Message, 2>::run,
					  "producer",
		              configMINIMAL_STACK_SIZE,
		              &prod,
		              tskIDLE_PRIORITY + 1,
		              &prod_handle);

	if (ret != pdPASS)
	{
		return;
	}

	if (xTaskCreate(Consumer<Message, 2>::run, "consumer", configMINIMAL_STACK_SIZE, &con, tskIDLE_PRIORITY + 1, &con_handle) != pdPASS)
	{
		return;
	}

    vTaskStartScheduler();
	printf("debug ---> %s\n", __func__);

    for (;;)
    {
		printf("debug ---> %s\n", __func__);
    }
}
