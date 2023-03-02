#include "MemoryManager.h"
#include <malloc.h>


CMemoryManager::CMemoryManager(int _nMemorySize)
{
	m_pFreeMemoryStart = malloc(_nMemorySize);
}

void* CMemoryManager::Allocate(int nSize)
{
	void* pResult = nullptr;

	if(m_nMemoryUsed + nSize <= m_nMemorySize)
	{
		m_nMemoryUsed += nSize;
		pResult = m_pFreeMemoryStart;

		m_pFreeMemoryStart = reinterpret_cast<char*>( m_pFreeMemoryStart ) + nSize;
	}

	return pResult;
}

int CMemoryManager::Release(void* pMemAddr, int nSize)
{
	return 0;
}

CMemoryManager::~CMemoryManager()
{
}
