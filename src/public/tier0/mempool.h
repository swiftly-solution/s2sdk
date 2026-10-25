#ifndef MEMPOOL_H
#define MEMPOOL_H

#ifdef _WIN32
#pragma once
#endif

#include "memalloc.h"
#include "platform.h"
#include "threadtools.h"
#include "tslist.h"
#include "rawallocator.h"
#include "utlvector.h"
#include "utlleanvector.h"
#include "public/mathlib/mathlib.h"

enum MemoryPoolGrowType_t
{
	UTLMEMORYPOOL_GROW_NONE=0,	// Don't allow new blobs.
	UTLMEMORYPOOL_GROW_FAST=1,	// New blob size is numElements * (i+1)  (ie: the blocks it allocates get larger and larger each time it allocates one).
	UTLMEMORYPOOL_GROW_SLOW=2,	// New blob size is numElements.
};

class CUtlMemoryPoolBase
{
public:
	DLL_CLASS_IMPORT			CUtlMemoryPoolBase( int blockSize, int numElements, int nAlignment = 0, MemoryPoolGrowType_t growMode = UTLMEMORYPOOL_GROW_FAST, const char *pszAllocOwner = NULL, MemAllocAttribute_t allocAttribute = MemAllocAttribute_Unk0 );
	DLL_CLASS_IMPORT			~CUtlMemoryPoolBase();

	// Resets the pool
	DLL_CLASS_IMPORT void		Init( int blockSize, int numElements, int nAlignment, MemoryPoolGrowType_t growMode, const char *pszAllocOwner, MemAllocAttribute_t allocAttribute );

	DLL_CLASS_IMPORT void*		Alloc();	// Allocate the element size you specified in the constructor.
	DLL_CLASS_IMPORT void*		AllocZero();	// Allocate the element size you specified in the constructor, zero the memory before construction
	DLL_CLASS_IMPORT void		Free( void *pMem );
	
	// Frees everything
	void Clear() { ClearDestruct( NULL ); }
	
	// returns number of allocated blocks
	int Count() const		{ return m_BlocksAllocated; }
	int PeakCount() const	{ return m_PeakAlloc; }
	int BlockSize() const	{ return m_BlockSize; }
	int Size() const		{ return m_TotalSize; }

	DLL_CLASS_IMPORT bool		IsAllocationWithinPool( void *pMem ) const;

	// Calls pfnCallback with each allocated element and pContext
	typedef void (*IterateAllocationsFunc_t)( void *pElement, void *pContext );
	DLL_CLASS_IMPORT void		IterateAllocationsUnsafe( IterateAllocationsFunc_t pfnCallback, void *pContext );

protected:
	DLL_CLASS_IMPORT void		ClearDestruct( void (*)( void* ) );

private:
	class CBlob
	{
	public:
		CBlob	*m_pNext;
		int		m_NumBytes; // Number of bytes in this blob.
		char	m_Data[1];
		char	m_Padding[3]; // to int align the struct
	};

	DLL_CLASS_IMPORT bool AddNewBlob();
	DLL_CLASS_IMPORT void ResetAllocationCounts();

	int			m_BlockSize;
	int			m_BlocksPerBlob;

	MemoryPoolGrowType_t m_GrowMode;

	CInterlockedInt	m_BlocksAllocated;
	CInterlockedInt	m_PeakAlloc;
	unsigned short	m_nAlignment;
	unsigned short	m_NumBlobs;
	
	CTSListBase		m_FreeBlocks;
	
	MemAllocAttribute_t m_AllocAttribute;
	
	bool 			m_Unk1;
	CAtomicMutex	m_Mutex;
	CBlob*			m_pBlobHead;
	int				m_TotalSize;
};


//-----------------------------------------------------------------------------
// Wrapper macro to make an allocator that returns particular typed allocations
// and construction and destruction of objects.
//-----------------------------------------------------------------------------
template< class T >
class CUtlMemoryPool : public CUtlMemoryPoolBase
{
public:
	CUtlMemoryPool( int numElements, MemoryPoolGrowType_t growMode = UTLMEMORYPOOL_GROW_FAST, const char *pszAllocOwner = nullptr, MemAllocAttribute_t allocAttribute = MemAllocAttribute_Unk0 ) 
		: CUtlMemoryPoolBase( sizeof(T), numElements, alignof(T), growMode, pszAllocOwner, allocAttribute ) {}

	T*		Alloc();
	T*		AllocZero();
	void	Free( T *pMem );
	void	Clear();
};


//-----------------------------------------------------------------------------
// Multi-thread/Thread Safe Memory Class
//-----------------------------------------------------------------------------
template< class T >
using CUtlMemoryPoolMT = CUtlMemoryPool<T>;


//-----------------------------------------------------------------------------
// Specialized pool for aligned data management
//-----------------------------------------------------------------------------
template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, bool GROWMODE = false, int COMPACT_THRESHOLD = 4 >
class CAlignedMemPool
{
	enum
	{
		BLOCK_SIZE = COMPILETIME_MAX( ALIGN_VALUE( ITEM_SIZE, ALIGNMENT ), 8 ),
	};

public:
	CAlignedMemPool();

	void *Alloc();
	void Free( void *p );

	static int __cdecl CompareChunk( void * const *ppLeft, void * const *ppRight );
	void Compact();

	int NumTotal()			{ AUTO_LOCK( m_mutex ); return m_Chunks.Count() * ( CHUNK_SIZE / BLOCK_SIZE ); }
	int NumAllocated()		{ AUTO_LOCK( m_mutex ); return NumTotal() - m_nFree; }
	int NumFree()			{ AUTO_LOCK( m_mutex ); return m_nFree; }

	int BytesTotal()		{ AUTO_LOCK( m_mutex ); return NumTotal() * BLOCK_SIZE; }
	int BytesAllocated()	{ AUTO_LOCK( m_mutex ); return NumAllocated() * BLOCK_SIZE; }
	int BytesFree()			{ AUTO_LOCK( m_mutex ); return NumFree() * BLOCK_SIZE; }

	int ItemSize()			{ return ITEM_SIZE; }
	int BlockSize()			{ return BLOCK_SIZE; }
	int ChunkSize()			{ return CHUNK_SIZE; }

private:
	struct FreeBlock_t
	{
		FreeBlock_t *pNext;
		uint8_t		reserved[ BLOCK_SIZE - sizeof( FreeBlock_t *) ];
	};

	CUtlVector<void *>	m_Chunks;		// Chunks are tracked outside blocks (unlike CUtlMemoryPool) to simplify alignment issues
	FreeBlock_t *		m_pFirstFree;
	int					m_nFree;
	CAllocator			m_Allocator;
	float				m_TimeLastCompact;

	CThreadFastMutex	m_mutex;
};

//-----------------------------------------------------------------------------
// Pool variant using standard allocation
//-----------------------------------------------------------------------------
template <typename T, int nInitialCount = 0, bool bDefCreateNewIfEmpty = true >
class CObjectPool
{
public:
	CObjectPool()
	{
		int i = nInitialCount;
		while ( i-- > 0 )
		{
			m_AvailableObjects.PushItem( new T );
		}
	}

	~CObjectPool()
	{
		Purge();
	}

	int NumAvailable()
	{
		return m_AvailableObjects.Count();
	}

	void Purge()
	{
		T *p;
		while ( m_AvailableObjects.PopItem( &p ) )
		{
			delete p;
		}
	}

	T *GetObject( bool bCreateNewIfEmpty = bDefCreateNewIfEmpty )
	{
		T *p;
		if ( !m_AvailableObjects.PopItem( &p )  )
		{
			p = ( bCreateNewIfEmpty ) ? new T : NULL;
		}
		return p;
	}

	void PutObject( T *p )
	{
		m_AvailableObjects.PushItem( p );
	}

private:
	CTSList<T *> m_AvailableObjects;
};

//-----------------------------------------------------------------------------
// Fixed budget pool with overflow to malloc
//-----------------------------------------------------------------------------
template <size_t PROVIDED_ITEM_SIZE, int ITEM_COUNT>
class CFixedBudgetMemoryPool
{
public:
	CFixedBudgetMemoryPool()
	{
		m_pBase = m_pLimit = 0;
		static_assert( ITEM_SIZE % 4 == 0 );
	}

	bool Owns( void *p )
	{
		return ( p >= m_pBase && p < m_pLimit );
	}

	void *Alloc()
	{
		if ( !m_pBase )
		{
			LOCAL_THREAD_LOCK();
			if ( !m_pBase )
			{
				uint8_t *pMemory = m_pBase = (uint8_t *)malloc( ITEM_COUNT * ITEM_SIZE );
				m_pLimit = m_pBase + ( ITEM_COUNT * ITEM_SIZE );

				for ( int i = 0; i < ITEM_COUNT; i++ )
				{
					m_freeList.Push( (TSLNodeBase_t *)pMemory );
					pMemory += ITEM_SIZE;
				}
			}
		}

		void *p = m_freeList.Pop();
		if ( p )
			return p;

		return malloc( ITEM_SIZE );
	}

	void Free( void *p )
	{
		if ( Owns( p ) )
			m_freeList.Push( (TSLNodeBase_t *)p );
		else
			free( p );
	}

	enum
	{
		ITEM_SIZE = ALIGN_VALUE( PROVIDED_ITEM_SIZE, 4 )
	};

	CTSListBase m_freeList;
	uint8_t *m_pBase;
	uint8_t *m_pLimit;
};

#define BIND_TO_FIXED_BUDGET_POOL( poolName )									\
	inline void* operator new( size_t size ) { return poolName.Alloc(); }   \
	inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine ) { return poolName.Alloc(); }   \
	inline void  operator delete( void* p ) { poolName.Free(p); }		\
	inline void  operator delete( void* p, int nBlockUse, const char *pFileName, int nLine ) { poolName.Free(p); }

//-----------------------------------------------------------------------------


template< class T >
inline T* CUtlMemoryPool<T>::Alloc()
{
	T *pRet;

	{
	pRet = (T*)CUtlMemoryPoolBase::Alloc();
	}

	if ( pRet )
	{
		Construct( pRet );
	}
	return pRet;
}

template< class T >
inline T* CUtlMemoryPool<T>::AllocZero()
{
	T *pRet;

	{
	pRet = (T*)CUtlMemoryPoolBase::AllocZero();
	}

	if ( pRet )
	{
		Construct( pRet );
	}
	return pRet;
}

template< class T >
inline void CUtlMemoryPool<T>::Free(T *pMem)
{
	if ( pMem )
	{
		Destruct( pMem );
	}

	CUtlMemoryPoolBase::Free( pMem );
}

template< class T >
inline void CUtlMemoryPool<T>::Clear()
{
	CUtlMemoryPoolBase::ClearDestruct( (void (*)( void* ))static_cast<void (*)( T* )>( &Destruct<T> ) );
}

//-----------------------------------------------------------------------------
// Macros that make it simple to make a class use a fixed-size allocator
// Put DECLARE_FIXEDSIZE_ALLOCATOR in the private section of a class,
// Put DEFINE_FIXEDSIZE_ALLOCATOR in the CPP file
//-----------------------------------------------------------------------------
#define DECLARE_FIXEDSIZE_ALLOCATOR( _class )									\
	public:																		\
		inline void* operator new( size_t size ) { MEM_ALLOC_CREDIT_(#_class " CUtlMemoryPool"); return s_Allocator.Alloc(); }   \
		inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine ) { MEM_ALLOC_CREDIT_(#_class " CUtlMemoryPool"); return s_Allocator.Alloc(); }   \
		inline void  operator delete( void* p ) { s_Allocator.Free((_class*)p); }		\
		inline void  operator delete( void* p, int nBlockUse, const char *pFileName, int nLine ) { s_Allocator.Free((_class*)p); }   \
	private:																		\
		static   CUtlMemoryPool<_class>   s_Allocator
    
#define DEFINE_FIXEDSIZE_ALLOCATOR( _class, _initsize, _grow )					\
	CUtlMemoryPool<_class>   _class::s_Allocator(_initsize, _grow, #_class " CUtlMemoryPool", MemAllocAttribute_Unk2)

#define DECLARE_FIXEDSIZE_ALLOCATOR_MT( _class )									\
	public:																		\
	   inline void* operator new( size_t size ) { MEM_ALLOC_CREDIT_(#_class " CUtlMemoryPoolMT"); return s_Allocator.Alloc(); }   \
	   inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine ) { MEM_ALLOC_CREDIT_(#_class " CUtlMemoryPoolMT"); return s_Allocator.Alloc(); }   \
	   inline void  operator delete( void* p ) { s_Allocator.Free((_class*)p); }		\
	   inline void  operator delete( void* p, int nBlockUse, const char *pFileName, int nLine ) { s_Allocator.Free((_class*)p); }   \
	private:																		\
		static   CUtlMemoryPoolMT<_class>   s_Allocator

#define DEFINE_FIXEDSIZE_ALLOCATOR_MT( _class, _initsize, _grow )					\
	CUtlMemoryPoolMT<_class>   _class::s_Allocator(_initsize, _grow, #_class " CUtlMemoryPoolMT", MemAllocAttribute_Unk2)

//-----------------------------------------------------------------------------
// Macros that make it simple to make a class use a fixed-size allocator
// This version allows us to use a memory pool which is externally defined...
// Put DECLARE_FIXEDSIZE_ALLOCATOR_EXTERNAL in the private section of a class,
// Put DEFINE_FIXEDSIZE_ALLOCATOR_EXTERNAL in the CPP file
//-----------------------------------------------------------------------------

#define DECLARE_FIXEDSIZE_ALLOCATOR_EXTERNAL( _class )							\
   public:																		\
      inline void* operator new( size_t size )  { MEM_ALLOC_CREDIT_(#_class " CUtlMemoryPool"); return s_pAllocator->Alloc(); }   \
      inline void* operator new( size_t size, int nBlockUse, const char *pFileName, int nLine )  { MEM_ALLOC_CREDIT_(#_class " CUtlMemoryPool"); return s_pAllocator->Alloc(); }   \
      inline void  operator delete( void* p )   { s_pAllocator->Free((_class*)p); }		\
   private:																		\
      static   CUtlMemoryPool<_class>*   s_pAllocator

#define DEFINE_FIXEDSIZE_ALLOCATOR_EXTERNAL( _class, _allocator )				\
   CUtlMemoryPool<_class>*   _class::s_pAllocator = _allocator


template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, bool GROWMODE, int COMPACT_THRESHOLD >
inline CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, GROWMODE, COMPACT_THRESHOLD>::CAlignedMemPool()
  : m_pFirstFree( 0 ),
	m_nFree( 0 ),
	m_TimeLastCompact( 0 )
{
	static_assert( sizeof( FreeBlock_t ) >= BLOCK_SIZE );
	static_assert( ALIGN_VALUE( sizeof( FreeBlock_t ), ALIGNMENT ) == sizeof( FreeBlock_t ) );
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, bool GROWMODE, int COMPACT_THRESHOLD >
inline void *CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, GROWMODE, COMPACT_THRESHOLD>::Alloc()
{
	AUTO_LOCK( m_mutex );

	if ( !m_pFirstFree )
	{
		if ( !GROWMODE && m_Chunks.Count() )
		{
			return NULL;
		}

		FreeBlock_t *pNew = (FreeBlock_t *)m_Allocator.Alloc( CHUNK_SIZE );
		Assert( (uintp)pNew % ALIGNMENT == 0 );
		m_Chunks.AddToTail( pNew );
		m_nFree = CHUNK_SIZE / BLOCK_SIZE;
		m_pFirstFree = pNew;
		for ( int i = 0; i < m_nFree - 1; i++ )
		{
			pNew->pNext = pNew + 1;
			pNew++;
		}
		pNew->pNext = NULL;
	}

	void *p = m_pFirstFree;
	m_pFirstFree = m_pFirstFree->pNext;
	m_nFree--;

	return p;
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, bool GROWMODE, int COMPACT_THRESHOLD >
inline void CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, GROWMODE, COMPACT_THRESHOLD>::Free( void *p )
{
	AUTO_LOCK( m_mutex ); 

	// Insertion sort to encourage allocation clusters in chunks
	FreeBlock_t *pFree = ((FreeBlock_t *)p);
	FreeBlock_t *pCur = m_pFirstFree;
	FreeBlock_t *pPrev = NULL;

	while ( pCur && pFree > pCur )
	{
		pPrev = pCur;
		pCur = pCur->pNext;
	}

	pFree->pNext = pCur;

	if ( pPrev )
	{
		pPrev->pNext = pFree;
	}
	else
	{
		m_pFirstFree = pFree;
	}
	m_nFree++;

	if ( m_nFree >= ( CHUNK_SIZE / BLOCK_SIZE ) * COMPACT_THRESHOLD )
	{
		float time = Plat_FloatTime();
		float compactTime = ( m_nFree >= ( CHUNK_SIZE / BLOCK_SIZE ) * COMPACT_THRESHOLD * 4 ) ? 15.0 : 30.0;
		if ( m_TimeLastCompact > time || m_TimeLastCompact + compactTime < Plat_FloatTime() )
		{
			Compact();
			m_TimeLastCompact = time;
		}
	}
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, bool GROWMODE, int COMPACT_THRESHOLD >
inline int __cdecl CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, GROWMODE, COMPACT_THRESHOLD>::CompareChunk( void * const *ppLeft, void * const *ppRight )
{
	return ((uintp)*ppLeft) - ((uintp)*ppRight);
}

template <int ITEM_SIZE, int ALIGNMENT, int CHUNK_SIZE, class CAllocator, bool GROWMODE, int COMPACT_THRESHOLD >
inline void CAlignedMemPool<ITEM_SIZE, ALIGNMENT, CHUNK_SIZE, CAllocator, GROWMODE, COMPACT_THRESHOLD>::Compact()
{
	FreeBlock_t *pCur = m_pFirstFree;
	FreeBlock_t *pPrev = NULL;

	m_Chunks.Sort( CompareChunk );

#ifdef VALIDATE_ALIGNED_MEM_POOL
	{
		FreeBlock_t *p = m_pFirstFree;
		while ( p )
		{
			if ( p->pNext && p > p->pNext )
			{
				__asm { int 3 }
			}
			p = p->pNext;
		}

		for ( int i = 0; i < m_Chunks.Count(); i++ )
		{
			if ( i + 1 < m_Chunks.Count() )
			{
				if ( m_Chunks[i] > m_Chunks[i + 1] )
				{
					__asm { int 3 }
				}
			}
		}
	}
#endif

	int i;

	for ( i = 0; i < m_Chunks.Count(); i++ )
	{
		int nBlocksPerChunk = CHUNK_SIZE / BLOCK_SIZE;
		FreeBlock_t *pChunkLimit = ((FreeBlock_t *)m_Chunks[i]) + nBlocksPerChunk;
		int nFromChunk = 0;
		if ( pCur == m_Chunks[i] )
		{
			FreeBlock_t *pFirst = pCur;
			while ( pCur && pCur >= m_Chunks[i] && pCur < pChunkLimit )
			{
				pCur = pCur->pNext;
				nFromChunk++;
			}
			pCur = pFirst;

		}

		while ( pCur && pCur >= m_Chunks[i] && pCur < pChunkLimit )
		{
			if ( nFromChunk != nBlocksPerChunk )
			{
				if ( pPrev )
				{
					pPrev->pNext = pCur;
				}
				else
				{
					m_pFirstFree = pCur;
				}
				pPrev = pCur;
			}
			else if ( pPrev )
			{
				pPrev->pNext = NULL;
			}
			else
			{
				m_pFirstFree = NULL;
			}

			pCur = pCur->pNext;
		}

		if ( nFromChunk == nBlocksPerChunk )
		{
			m_Allocator.Free( m_Chunks[i] );
			m_nFree -= nBlocksPerChunk;
			m_Chunks[i] = 0;
		}
	}

	for ( i = m_Chunks.Count() - 1; i >= 0 ; i-- )
	{
		if ( !m_Chunks[i] )
		{
			m_Chunks.FastRemove( i );
		}
	}
}

// AMNOTE: Handle that contains page/subpage indexes to allocated memory within internal storage
// Stored in the following format: 
// offset within page of size MEMBLOCK_PAGEOFFSET_BIT
// page index of size MEMBLOCK_PAGEINDEX_BIT
typedef unsigned int MemBlockHandle_t;

#define MEMBLOCKHANDLE_INVALID ((MemBlockHandle_t)~0)

// Sizes in bytes, the allocator divides them by sizeof( T )
#define MEMBLOCK_DEFAULT_PAGESIZE (0x800)
#define MEMBLOCK_PAGESIZE_SECTION1 (0x10000)
#define MEMBLOCK_PAGESIZE_SECTION2 (0x80000)
#define MEMBLOCK_MAX_TOTAL_PAGESIZE (0x200000)

#define MEMBLOCK_PAGEOFFSET_BIT (21)
#define MEMBLOCK_PAGEOFFSET_MASK ((1 << MEMBLOCK_PAGEOFFSET_BIT) - 1)

// Page sizes, offsets and allocation sizes are counted in elements of T
template<class T, class A = CMemAllocAllocator>
class CUtlMemoryBlockAllocator
{
	typedef A CAllocator;

public:
	CUtlMemoryBlockAllocator( int nInitPages = 0, uint32 nPageSize = MEMBLOCK_DEFAULT_PAGESIZE / sizeof( T ), uint32 nMaxPageSize = MEMBLOCK_MAX_TOTAL_PAGESIZE / sizeof( T ), bool bStaticPageSize = false ) :
		m_nPageOffsetBits( MEMBLOCK_PAGEOFFSET_BIT ),
		m_nPageOffsetMask( MEMBLOCK_PAGEOFFSET_MASK ),
		m_MemPages( 0, nInitPages ),
		m_nPageSize( nPageSize ),
		m_bStaticPageSize( bStaticPageSize ),
		m_bAllocFromLastPageOnly( false )
	{
		SetPageSize( nPageSize, nMaxPageSize, bStaticPageSize );
	}

	~CUtlMemoryBlockAllocator( void )
	{
		Purge();
	}

	uint32 MaxPageSize() const { return m_bStaticPageSize ? m_nPageSize : (m_nPageOffsetMask + 1); }
	uint32 MaxPossiblePageSize() const { return (1 << (32 - m_nPageOffsetBits)); }

	// Clears all memory buffers preserving only nSize elements, which would be treated like a fresh memory
	void RemoveAll( size_t nSize = 0 );
	void Purge( void );

	void Swap( CUtlMemoryBlockAllocator<T, A> &other );

	MemBlockHandle_t Alloc( unsigned int nSize );

	// Allocates nCount elements on the last pages, so their handles are consecutive indexes,
	// and returns how many were allocated, which includes the rest of the last page if bFillLastPage
	unsigned int Grow( unsigned int nCount, bool bFillLastPage );

	size_t MemUsage( void ) const;

	void SetPageSize( uint32 nPageSize = MEMBLOCK_DEFAULT_PAGESIZE / sizeof( T ), uint32 nMaxPageSize = MEMBLOCK_MAX_TOTAL_PAGESIZE / sizeof( T ), bool bStaticPageSize = false );

	void* GetBlock( MemBlockHandle_t handle ) const;

public:
	int AddPage( unsigned int nCount );

	MemBlockHandle_t CreateHandle( int page_idx ) const
	{
		Assert( page_idx >= 0 && page_idx < m_MemPages.Count() );
		return m_MemPages[page_idx].m_nUsedSize | (page_idx << m_nPageOffsetBits);
	}

	int GetPageIdxFromHandle( MemBlockHandle_t handle ) const { return handle >> m_nPageOffsetBits; }
	int GetPageOffsetFromHandle( MemBlockHandle_t handle ) const { return handle & m_nPageOffsetMask; }

	unsigned int CalcPageSize( int page_idx, unsigned int requested_size ) const;
	int FindPageWithSpace( unsigned int nSize ) const;

	struct MemPage_t
	{
		unsigned int MemoryLeft() const { return m_nTotalSize - m_nUsedSize; }

		unsigned int	m_nTotalSize = 0;
		unsigned int	m_nUsedSize = 0;
		T				*m_pMemory = nullptr;
	};

	typedef CUtlLeanVector<MemPage_t> MemPagesVec_t;

	unsigned int			m_nPageOffsetBits;
	unsigned int			m_nPageOffsetMask;
	MemPagesVec_t			m_MemPages;
	unsigned int			m_nPageSize;
	bool					m_bStaticPageSize;
	bool					m_bAllocFromLastPageOnly;
};

template<class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::RemoveAll( size_t nSize )
{
	size_t accumulated_total = 0;
	int removed_at = -1;

	FOR_EACH_VEC( m_MemPages, i )
	{
		accumulated_total += m_MemPages[i].m_nTotalSize;

		if(removed_at != -1 || (nSize && accumulated_total > nSize))
		{
			CAllocator::Free( m_MemPages[i].m_pMemory );

			if(removed_at == -1)
				removed_at = i;
		}
		else
		{
			m_MemPages[i].m_nUsedSize = 0;
		}
	}

	if(removed_at != -1)
	{
		m_MemPages.RemoveMultipleFromTail( m_MemPages.Count() - removed_at );
	}
}

template<class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::Purge( void )
{
	FOR_EACH_VEC( m_MemPages, i )
	{
		CAllocator::Free( m_MemPages[i].m_pMemory );
	}

	m_MemPages.Purge();
}

template<class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::Swap( CUtlMemoryBlockAllocator<T, A> &other )
{
	V_swap( m_nPageOffsetBits, other.m_nPageOffsetBits );
	V_swap( m_nPageOffsetMask, other.m_nPageOffsetMask );
	m_MemPages.Swap( other.m_MemPages );
	V_swap( m_nPageSize, other.m_nPageSize );
	V_swap( m_bStaticPageSize, other.m_bStaticPageSize );
	V_swap( m_bAllocFromLastPageOnly, other.m_bAllocFromLastPageOnly );
}

template<class T, class A>
inline int CUtlMemoryBlockAllocator<T, A>::AddPage( unsigned int nSize )
{
	if((uint32)m_MemPages.Count() >= MaxPossiblePageSize())
	{
		Plat_FatalError( "%s: no space for allocation of %u\n", __FUNCTION__, nSize );
		DebuggerBreak();
	}

	int page_idx = m_MemPages.AddToTail();
	auto &page = m_MemPages[page_idx];

	uint32 alloced_page_size = 0;
	page.m_pMemory = CAllocator::template Alloc<T>( CalcPageSize( page_idx, nSize ), alloced_page_size );
	page.m_nUsedSize = 0;

	if(m_bStaticPageSize)
		page.m_nTotalSize = m_nPageSize;
	else
		page.m_nTotalSize = MIN( alloced_page_size, MaxPageSize() );

	return page_idx;
}

template<class T, class A>
inline MemBlockHandle_t CUtlMemoryBlockAllocator<T, A>::Alloc( unsigned int nSize )
{
	if(nSize > MaxPageSize())
	{
		Plat_FatalError( "%s: allocation of %u too large, max %u\n", __FUNCTION__, nSize, MaxPageSize() );
		DebuggerBreak();
	}

	int page_idx = FindPageWithSpace( nSize );

	// Allocate new page since we can't fit in existing ones
	if(page_idx == -1)
		page_idx = AddPage( nSize );

	MemBlockHandle_t handle = CreateHandle( page_idx );
	m_MemPages[page_idx].m_nUsedSize += nSize;

	return handle;
}

template<class T, class A>
inline unsigned int CUtlMemoryBlockAllocator<T, A>::Grow( unsigned int nCount, bool bFillLastPage )
{
	if(nCount == 0)
		return 0;

	unsigned int nAllocated = 0;
	MemPage_t *pPage = m_MemPages.Count() > 0 ? &m_MemPages.Tail() : nullptr;

	while(nCount > 0)
	{
		if(!pPage || pPage->MemoryLeft() == 0)
			pPage = &m_MemPages[AddPage( MIN( nCount, MaxPageSize() ) )];

		unsigned int nUsed = MIN( nCount, pPage->MemoryLeft() );
		pPage->m_nUsedSize += nUsed;
		nAllocated += nUsed;
		nCount -= nUsed;
	}

	if(bFillLastPage)
	{
		nAllocated += pPage->MemoryLeft();
		pPage->m_nUsedSize = pPage->m_nTotalSize;
	}

	return nAllocated;
}

template<class T, class A>
inline size_t CUtlMemoryBlockAllocator<T, A>::MemUsage( void ) const
{
	size_t mem_usage = 0;

	FOR_EACH_VEC( m_MemPages, i )
	{
		mem_usage += m_MemPages[i].m_nTotalSize;
	}

	return mem_usage;
}

template<class T, class A>
inline void CUtlMemoryBlockAllocator<T, A>::SetPageSize( uint32 nPageSize, uint32 nMaxPageSize, bool bStaticPageSize )
{
	RemoveAll();

	m_nPageSize = nPageSize;
	m_bStaticPageSize = bStaticPageSize;

	m_nPageOffsetBits = 1;
	m_nPageOffsetMask = SmallestPowerOfTwoGreaterOrEqual( nMaxPageSize ) - 1;

	if(m_nPageOffsetMask >= 2)
	{
		uint32 largest_bit = 1;
		uint32 temp_val = (1 << largest_bit);

		while(temp_val <= m_nPageOffsetMask)
		{
			temp_val = (1 << ++largest_bit);
		}

		m_nPageOffsetBits = largest_bit;
	}

	int nMaxPages = (int)MaxPossiblePageSize();
	if(m_MemPages.Count() > nMaxPages)
	{
		for(int i = nMaxPages; i < m_MemPages.Count(); i++)
			CAllocator::Free( m_MemPages[i].m_pMemory );

		m_MemPages.RemoveMultipleFromTail( m_MemPages.Count() - nMaxPages );
	}
}

template<class T, class A>
inline unsigned int CUtlMemoryBlockAllocator<T, A>::CalcPageSize( int page_idx, unsigned int requested_size ) const
{
	if(m_bStaticPageSize)
		return m_nPageSize;

	uint32 page_size = MEMBLOCK_DEFAULT_PAGESIZE / sizeof( T );
	if(page_idx >= 8)
	{
		if(page_idx >= 16)
			page_size = MEMBLOCK_PAGESIZE_SECTION2 / sizeof( T );
		else
			page_size = MEMBLOCK_PAGESIZE_SECTION1 / sizeof( T );
	}

	page_size = MIN( page_size, m_nPageOffsetMask + 1 );
	page_size = MAX( page_size, m_nPageSize );
	return MAX( page_size, requested_size );
}

template<class T, class A>
inline int CUtlMemoryBlockAllocator<T, A>::FindPageWithSpace( unsigned int nSize ) const
{
	int last_idx = m_MemPages.Count() - 1;
	if(last_idx < 0)
		return -1;

	if(nSize < m_MemPages[last_idx].MemoryLeft())
		return last_idx;

	if(!m_bAllocFromLastPageOnly)
	{
		for(int i = 0; i < last_idx; i++)
		{
			if(nSize <= m_MemPages[i].MemoryLeft())
				return i;
		}
	}

	return -1;
}

template<class T, class A>
inline void* CUtlMemoryBlockAllocator<T, A>::GetBlock( MemBlockHandle_t handle ) const
{
	int nPageIndex = GetPageIdxFromHandle( handle );
	int nPageOffset = GetPageOffsetFromHandle( handle );

	if ( nPageIndex >= 0 && nPageIndex < m_MemPages.Count() )
		return (void*)&m_MemPages[ nPageIndex ].m_pMemory[ nPageOffset ];

	return NULL;
}

struct UtlScratchMemoryPoolMark_t
{
	void *m_pBlock;
	int m_nBytesFree;
};

// Chunked memory allocator of a size (m_nBlockSize), where first sizeof(MemoryBlock_t) bytes are allocated for 
// MemoryBlock_t struct, that holds additional information about next chunk in a linked-list manner.
class CUtlScratchMemoryPool
{
public:
	DLL_CLASS_IMPORT CUtlScratchMemoryPool();

	// Creates a scratch memory pool with set blocksize, already allocated memory chunk could be provided via pExternalMem, where nAllocatedBlockSize
	// is a size of an allocated memory, passing the memory of size less than sizeof(MemoryBlock_t) bytes (required for the header) might lead to OOB access.
	// If bSearchAllBlocks is true, it would search all the blocks to find free memory to use, alternatively it would allocate new block if
	// the current one can't fit the required memory.
	DLL_CLASS_IMPORT CUtlScratchMemoryPool( uint32 nBlockSize, uint32 nAllocatedBlockSize = 0, void *pExternalMem = NULL, bool bSearchAllBlocks = true );
	DLL_CLASS_IMPORT ~CUtlScratchMemoryPool();

	// Initializes scratch memory pool with set blocksize, already allocated memory chunk could be provided via pExternalMem, where nAllocatedBlockSize
	// is a size of an allocated memory, passing the memory of size less than sizeof(MemoryBlock_t) bytes (required for the header) might lead to OOB access.
	// If bSearchAllBlocks is true, it would search all the blocks to find free memory to use, alternatively it would allocate new block if
	// the current one can't fit the required memory.
	DLL_CLASS_IMPORT void Init( uint32 nBlockSize, uint32 nAllocatedBlockSize = 0, void *pExternalMem = NULL, bool bSearchAllBlocks = true );

	// Returns info about currently active chunk of memory
	DLL_CLASS_IMPORT UtlScratchMemoryPoolMark_t GetCurrentAllocPoint() const;

	// Allocates a memory within the current chunk, if it fits, or creates a new one instead (would search other chunks if m_bSearchAllBlocks == true)
	DLL_CLASS_IMPORT void *AllocAligned( int nSizeInBytes, int nAlignment );

	// Counts the overall allocated size of all the chunks within this memory pool
	DLL_CLASS_IMPORT size_t AllocSize() const;
	// Counts the overall amount of chunks allocated within this memory pool
	DLL_CLASS_IMPORT size_t AllocationCount() const;
	// Counts the overall free memory available across all the allocated chunks within this memory pool
	DLL_CLASS_IMPORT size_t TotalMemFree() const;

	// Frees all the allocated chunks, leaving only one active chunk
	DLL_CLASS_IMPORT void Purge();
	// Frees all the allocated chunks that lead to the mark, would result in a crash if the mark doesn't point to any of the chunks within this memory pool
	DLL_CLASS_IMPORT void FreeToAllocPoint( UtlScratchMemoryPoolMark_t mark );

protected:
	// Allocates new chunk of memory, if nSizeInBytes > (m_nBlockSize / 2) the allocated chunk would be allocated as is without free memory in it (even if it exceeds the m_nBlockSize).
	DLL_CLASS_IMPORT void *AddNewBlock( int nSizeInBytes );

private:
	struct ALIGN16 MemoryBlock_t
	{
		MemoryBlock_t *m_pNext;
		uint32 m_nBytesFree;
		uint32 m_nBlockSize;
		uint32 m_nAllocSize;
	} ALIGN16_POST;

	MemoryBlock_t *m_pFirstBlock;
	uint32 m_nBlockSize;
	bool m_bSearchAllBlocks;
};

template <size_t SIZE>
class CUtlScratchMemoryPoolFixedGrowable : public CUtlScratchMemoryPool
{
public:
	CUtlScratchMemoryPoolFixedGrowable( bool bSearchAllBlocks = true ) : CUtlScratchMemoryPool( SIZE, SIZE, &m_initialAllocationMemory, bSearchAllBlocks ) {}

private:
	unsigned char m_initialAllocationMemory[SIZE];
};

#endif