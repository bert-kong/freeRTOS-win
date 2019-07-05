
#ifndef __PRODUCER_COMSUMER_H__
#define __PRODUCER_COMSUMER_H__

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <string>


#define __USING_MUTEX__
namespace bk
{
	template<uint8_t N>
	class ProducerConsumer
	{
	public:
		struct BankAccount
		{
			BankAccount(const int i,
					    const int b,
				        const char* n)
				:id(i), balance(b)
			{
				::memcpy(name, n, 32);
			}

			int getBalance() const
			{
				return balance;
			}
;
			const char *getName() const
			{
				return name;
			}

			const int id;
			int balance;
			char name[32];
		};

	public:
		static const int stack_size;
		static const int priority;

		static void producer(void*);
		static void consumer(void*);

	public:
		ProducerConsumer();
		~ProducerConsumer()
		{
			::vQueueDelete(m_queue);
		}

		bool done() const
		{
			return m_done;
		}

		bool isQueueFull() const
		{
			return ::uxQueueSpacesAvailable(m_queue)==0;
		}

		bool isQueueEmpty() const
		{
			return ::uxQueueSpacesAvailable(m_queue) == m_capacity;
		}

		/**
		 * producer
		 */
		void generateData();
		void onSendToQueue();

		/**
		 * consumer
		 */
		void processData();
		void onReceiveFromQueue();


	private:
		const uint8_t m_capacity = N;
		QueueHandle_t m_queue;

		bool m_done;

		int m_id;
		int m_balance;
		int m_previous_value;
	};
};

#ifdef __cplusplus
extern "C"
void main_producer_consumer();
#endif

#endif // !__PRODUCER_COMSUMER_H__
