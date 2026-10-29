#include "public/tier0/mempool.h"

void CUtlMemoryPoolBase:: Clear()
{ ClearDestruct( NULL ); }

int CUtlMemoryPoolBase:: Count() const
{ return m_BlocksAllocated; }

int CUtlMemoryPoolBase:: PeakCount() const
{ return m_PeakAlloc; }

int CUtlMemoryPoolBase:: BlockSize() const
{ return m_BlockSize; }

int CUtlMemoryPoolBase:: Size() const
{ return m_TotalSize; }
