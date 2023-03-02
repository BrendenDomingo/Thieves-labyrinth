#include "Pool.h"


template<typename T, int N>
CFlaggedPool<T, N>::CFlaggedPool(T ptBase)
{
	m_ptObjects = new T[N];

	for(int i = 0; i < N; i++)
	{
		m_ptObjects[i] = ptBase;
	}

	m_btFlags.reset();
}

template<typename T, int N>
T* CFlaggedPool<T, N>::GetAvailableElement()
{
	for(int i = 0; i < N; i++)
	{
		if(m_btFlags.test(i))
		{
			return &m_ptObjects[i];
		}
	}
	return nullptr;
}

template<typename T, int N>
int CFlaggedPool<T, N>::FreeElement(T* ptElementToRemove)
{
	for(int i = 0; i < N; i++)
	{
		if(m_ptObjects[i] == T * ptElementToRemove)
		{
			m_btFlags.reset(i);
			return 0;
		}
	}

	return -1;
}


template<typename T, int N>
CFlaggedPool<T, N>::~CFlaggedPool()
{
}

CFreeListPool::CFreeListPool()
{
}

CFreeListPool::~CFreeListPool()
{
}
