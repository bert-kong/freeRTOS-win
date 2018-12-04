
#include<cstdio>
#include<cstdint>
#include "FreeRTOS.h"
#include "semphr.h"

template<typename T, uint8_t N>
class CircularBuffer
{
public:
	explicit
	CircularBuffer()
		:m_size(N),
		 m_rd(0),
		 m_wr(0),
		 m_lock(::xSemaphoreCreateMutex())
	{
	}

	~CircularBuffer()
	{
	}


	const bool put(const T &);
	const bool get(T &);

protected:
	inline const bool isFull() const
	{
		return m_rd == ((m_wr + 1) % m_size);
	}

	inline const bool isEmpty() const
	{
		return m_rd == m_wr;
	}

private:
	CircularBuffer(const CircularBuffer &);
	const CircularBuffer &operator=(const CircularBuffer &);


private:
	const uint8_t m_size;
	T m_buffer[N];

	uint8_t m_rd;
	uint8_t m_wr;
	SemaphoreHandle_t m_lock;
};


template<typename T, uint8_t N>
class Producer
{
public:
	enum
	{
		NUM_OF_SLOTS = N,
	};

public:
	static void run(void *data);

public:
	Producer(CircularBuffer<T, N> &queue)
		:m_queue(queue)
	{
	}

	

	const bool write(const T &);

private:
	CircularBuffer<T, N> &m_queue;
};


template<typename T, uint8_t N>
class Consumer
{
public:
	enum
	{
		NUM_OF_SLOTS = N,
	};

public:
	static void run(void *data);

public:
	Consumer(CircularBuffer<T, N> &queue)
		:m_queue(queue)
	{
	}

	const bool read(T &obj);

private:
	CircularBuffer<T, N> &m_queue;
};


void main_producer_consumer();