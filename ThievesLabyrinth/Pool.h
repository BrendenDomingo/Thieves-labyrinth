#pragma once
#include <bitset>

template <typename T, int N>
class CFlaggedPool
{
	std::bitset<N> m_btFlags;
	T* m_ptObjects;

public:
	CFlaggedPool(T ptBase);

	T* GetAvailableElement();

	int FreeElement(T* ptElementToRemove);

	~CFlaggedPool();
};

class CFreeListPool
{
public:
	CFreeListPool();

	~CFreeListPool();
};