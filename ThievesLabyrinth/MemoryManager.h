#pragma once

class CMemoryManager
{
	int m_nMemorySize;
	int m_nMemoryUsed;

	void* m_pFreeMemoryStart;

public:

	CMemoryManager(int nMemorySize);

	void* Allocate(int nSize);

	int Release(void* pMemAddr, int nSize);

	virtual~CMemoryManager();
};

