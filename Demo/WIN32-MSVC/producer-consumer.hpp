
#ifndef __PRODUCER_COMSUMER_H__
#define __PRODUCER_COMSUMER_H__

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>


#define __USING_MUTEX__
namespace bk
{
	template<typename T, int N=10>
	class Queue
	{
	public:
		struct Node
		{
			Node(const T& data)
				:next(nullptr), data_p(new T(data))
			{
			}
			~Node()
			{
				//::printf("debug ---> delete\n");
				delete data_p;
			}

			Node* next;
			T *data_p;
		};

	public:
		class iterator
		{
		public:
			iterator(const Node* node)
				:m_node(node)
			{
			}
			~iterator()
			{
			}
			
			// ++i
			iterator operator++(int)
			{
				m_node = m_node->next;
				return iterator(m_mode);
			}

			// i++
			iterator operator++()
			{
				Node* node = m_node;
				m_node = m_node->next;

				return iterator(node);
			}

			bool operator!=(const iterator& it)
			{
				return it.m_node != m_node;
			}

			bool operator==(const iterator& it)
			{
				return it.m_node == m_node;
			}

			T& operator*()
			{
				return *(m_node->data_p);
			}

			T* operator->()
			{
				return m_node->data_p;
			}

		private:
			Node* m_node;
		};

	public:
		explicit
		Queue()
			:m_size(0), m_head(nullptr), m_tail(nullptr),
			 m_mutex(::xSemaphoreCreateMutex())
		{
			/* create mutex */
			if (m_mutex == NULL)
			{
				::printf("failed to create mutex\n");
			}
		}

		~Queue()
		{
		}

		const int size() const
		{
			return m_size;
		}

		const T& front() const
		{
			return *(m_head->data_p);
		}

		const T& back() const
		{
			return *(m_tail->data_p);
		}

		inline bool is_full() const
		{
			return m_size >= m_capacity;
		}

		inline bool is_empty() const
		{
			return m_size == 0;
		}

		/* push back */
		int push_back(const T& data)
		{
			BaseType_t ret;

			/* full */
#ifdef __USING_MUTEX__
			ret = ::xSemaphoreTake(m_mutex, portMAX_DELAY);
#else
			taskENTER_CRITICAL();
#endif
			if (is_full())
			{
#ifdef __USING_MUTEX__
				xSemaphoreGive(m_mutex);
#else
				taskEXIT_CRITICAL();
#endif
				return -1;
			}

			Node* node = new Node(data);


			if (m_head)
			{
				m_tail->next = node;
				m_tail = node;
			}
			else
			{
				m_head = node;
				m_tail = node;
			}

			m_size += 1;
			//::printf("debug ---> producer queue size %d\n", m_size);
#ifdef __USING_MUTEX__
			::xSemaphoreGive(m_mutex);
#else
			taskEXIT_CRITICAL();
#endif
			return 0;
		}


		/* pop_front */
		int pop_front(T* data)
		{
#ifdef __USING_MUTEX__
			::xSemaphoreTake(m_mutex, portMAX_DELAY);
#else
			taskENTER_CRITICAL();
#endif
			if (is_empty() || data == nullptr)
			{
#ifdef __USING_MUTEX__
				::xSemaphoreGive(m_mutex);
#else
				taskEXIT_CRITICAL();
#endif
				return -1;
			}

			if (m_head)
			{
				Node* node = m_head;
				*data = *(node->data_p);
				m_head = m_head->next;
			    m_size -= 1;
				//printf("debug ---> customer queue size %d\n", m_size);
				delete node;
			}
#ifdef __USING_MUTEX__
			::xSemaphoreGive(m_mutex);
#else
			::taskEXIT_CRITICAL();
#endif

			return 0;
		}


		iterator begin()
		{
			return iterator(m_head);
		}

		iterator end()
		{
			return iterator(m_tail);
		}

	protected:
		const int m_capacity = N;
		int m_size;
		Node* m_head;
		Node* m_tail;
		SemaphoreHandle_t m_mutex;
	};

	
	template<typename value_t>
	class BankAccount
	{
	public:
		BankAccount(const value_t value = static_cast<value_t>(0))
			:m_amount(value)
		{
		}
		BankAccount(const BankAccount& ba)
			:m_amount(ba.m_amount)
		{
		}

		~BankAccount()
		{
			::printf("%s\n", __func__);
		}

		BankAccount& operator=(const BankAccount& ba)
		{
			if (this == &ba)
			{
				return *this;
			}

			m_amount = ba.m_amount;

			return *this;
		}

		void deposit(const value_t value)
		{
			m_amount = value;
		}

		value_t balance() const
		{
			return m_amount;
		}

	private:
		value_t m_amount;
	};

	class ProducerConsumer
	{
	public:
		ProducerConsumer()
		{
		}
		~ProducerConsumer()
		{
		}
	};

}

#ifdef __cplusplus
extern "C"
void producer_consumer();
#endif

#endif // !__PRODUCER_COMSUMER_H__
