#include "public/tier0/circularbuffer.h"

int CCircularBuffer:: GetSize() const
{ return m_nSize; }

int CCircularBuffer:: GetReadAvailable() const
{ return m_nCount; }

int CCircularBuffer:: GetWriteAvailable() const
{ return m_nSize - m_nCount; }
