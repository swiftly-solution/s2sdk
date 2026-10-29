#include "public/tier0/memstack.h"

const void * CMemoryStack::GetBase() const
{  return const_cast<CMemoryStack *>(this)->GetBase(); }

int CMemoryStack::GetMaxSize()
{ 
	return m_maxSize;
}

int CMemoryStack::GetUsed()
{ 
	return ( m_pNextAlloc - m_pBase ); 
}

void * CMemoryStack::GetBase()
{
	return m_pBase;
}

MemoryStackMark_t CMemoryStack::GetCurrentAllocPoint()
{
	return ( m_pNextAlloc - m_pBase );
}
