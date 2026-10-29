#include "public/tier0/tslist.h"

bool  ThreadInterlockedAssignIf64x128( volatile int128 *pDest, const int128 &value, const int128 &comperand )
{ return ThreadInterlockedAssignIf128( pDest, value, comperand ); }

void * CTSListBase::operator new( size_t size )
{
	CTSListBase *pNode = (CTSListBase *)MemAlloc_AllocAligned( size, TSLIST_HEAD_ALIGNMENT );
	return pNode;
}

void CTSListBase::operator delete( void *p)
{
	MemAlloc_FreeAligned( p );
}

void CTSListBase:: Push( TSLNodeBase_t *pNode )
{ InternalPush( pNode ); }

TSLNodeBase_t * CTSListBase::Pop()
{ return InternalPop(); }

TSLHead_t * CTSListBase::AccessUnprotected()
{
	return &m_Head;
}

int CTSListBase:: Count() const
{
	return m_Head.value.Depth;
}
