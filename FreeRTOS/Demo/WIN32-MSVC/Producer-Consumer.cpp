
#include "Producer-Consumer.hpp"
#include "task.h"

#define RUN_ITERATIONS 100000
static	TaskHandle_t prod_handle = NULL;
static	TaskHandle_t con_handle = NULL;

template<typename T, uint8_t N>
const bool CircularBuffer<T, N>::put(const T &obj)
{
    bool ret = false;

    while (::xSemaphoreTake(m_lock, (TickType_t)0)==pdTRUE)
    {
		if (isFull())
		{
			::xSemaphoreGive(m_lock);

			// set Consumer's notification value to 133
			::xTaskNotify(con_handle, 1<<16 | 1<<8, eSetBits);
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
	uint32_t notify_value_enter_action = 0;         // do nothing
	uint32_t notify_value_exit_action = 0xFFFFFFFF; // clear all bits
	uint32_t notification_value;
	TickType_t timeout = portMAX_DELAY; // 0xFF...

	while (::xSemaphoreTake(m_lock, (TickType_t) 0)==pdTRUE)
	{
		if (isEmpty())
		{
			::xSemaphoreGive(m_lock);
			while (::xTaskNotifyWait(notify_value_enter_action,
								  notify_value_exit_action,
								  &notification_value,
				                  timeout) == pdTRUE)
			{
				if (!(notification_value & ((1 << 16) | (1 << 8))))
				{
					continue;
				}
				break;
			}
			continue;
		}

        obj = m_buffer[m_rd];
        m_rd = (m_rd + 1U) % m_size;
        ret = true;
		::xSemaphoreGive(m_lock);
		break;
	}

#if 0
    while (::xSemaphoreTake(m_lock, (TickType_t)0) == pdTRUE)
    {
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
#endif

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

#if 0
void task_gui(void *data)
{
	uint32_t value = 0;
	uint32_t ret_value;
	TaskHandle_t dest_task = con_handle;
	const eNotifyAction action = eSetValueWithOverwrite;

	if (con_handle==NULL)
	{
		return;
	}

	for(;;)
	{
		// write value to the consumer task's notification value
		if (xTaskNotifyAndQuery(dest_task, 0, eNoAction, &ret_value)==pdPASS)
		{
			if (xTaskNotify(dest_task, &value, action)==pdPASS)
			{
				value += 1;
			}
		}
	}
}
#endif


void main_producer_consumer()
{
	CircularBuffer<Message, 2> queue;
	Producer<Message, 2> prod(queue);
	Consumer<Message, 2> con(queue);
	BaseType_t ret;

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
