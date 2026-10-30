#if defined( _WIN32 ) && !defined( STEAM ) && !defined( NO_MALLOC_OVERRIDE )
// crtdbg.h is needed for the _Crt* definitions below, but without _DEBUG it turns _malloc_dbg
// and friends into macros. It must be the first thing to include it, so do it before any other header.
#ifndef _DEBUG
#define _DEBUG 1
#define MEMOVERRIDE_UNDEF_DEBUG
#endif
#include <crtdbg.h>
#ifdef MEMOVERRIDE_UNDEF_DEBUG
#undef _DEBUG
#undef MEMOVERRIDE_UNDEF_DEBUG
#endif
#endif

//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Insert this file into all projects using the memory system
// It will cause that project to use the shader memory allocator
//
// $NoKeywords: $
//=============================================================================//


#if !defined(STEAM) && !defined(NO_MALLOC_OVERRIDE)

#if defined( _WIN32 )
#define WIN_32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "public/tier0/dbg.h"
#include "public/tier0/memalloc.h"
#include <new>
#include <string.h>
#include <stdio.h>
#include "public/tier0/memdbgoff.h"

#ifdef POSIX
#define __cdecl
#endif

inline void *AllocUnattributed( size_t nSize )
{
	return g_pMemAlloc->Alloc( nSize );
}

inline void *ReallocUnattributed( void *pMem, size_t nSize )
{
	return g_pMemAlloc->Realloc( pMem, nSize );
}

// Zeroes the memory past the old size of the allocation
inline void *RecallocUnattributed( void *pMem, size_t nSize )
{
	size_t nOldSize = pMem ? g_pMemAlloc->GetSize( pMem ) : 0;
	void *pMemOut = ReallocUnattributed( pMem, nSize );
	if ( pMemOut && nSize > nOldSize )
		memset( (char *)pMemOut + nOldSize, 0, nSize - nOldSize );
	return pMemOut;
}

//-----------------------------------------------------------------------------
// Standard functions in the CRT that we're going to override to call our allocator
//-----------------------------------------------------------------------------
#if defined(_WIN32) && !defined(_STATIC_LINKED)
// this magic only works under win32
// under linux this malloc() overrides the libc malloc() and so we
// end up in a recursion (as g_pMemAlloc->Alloc() calls malloc)
#if _MSC_VER >= 1900
#define ALLOC_CALL  __declspec(restrict)
#define FREE_CALL
#elif _MSC_VER >= 1400
#define ALLOC_CALL _CRTNOALIAS _CRTRESTRICT 
#define FREE_CALL _CRTNOALIAS 
#else
#define ALLOC_CALL
#define FREE_CALL
#endif

extern "C"
{
	
ALLOC_CALL void *malloc( size_t nSize )
{
	return AllocUnattributed( nSize );
}

FREE_CALL void free( void *pMem )
{
	g_pMemAlloc->Free( pMem );
}

ALLOC_CALL void *realloc( void *pMem, size_t nSize )
{
	return ReallocUnattributed( pMem, nSize );
}

ALLOC_CALL void *calloc( size_t nCount, size_t nElementSize )
{
	void *pMem = AllocUnattributed( nElementSize * nCount );
	if ( pMem )
		memset( pMem, 0, nElementSize * nCount );
	return pMem;
}

} // end extern "C"

//-----------------------------------------------------------------------------
// Non-standard MSVC functions that we're going to override to call our allocator
//-----------------------------------------------------------------------------
extern "C"
{

// 64-bit
#ifdef _WIN64
#if ( defined ( _MSC_VER ) && _MSC_VER >= 1900 )
	_CRTRESTRICT
#endif
void* __cdecl _malloc_base( size_t nSize )
{
	return AllocUnattributed( nSize );
}
#else
ALLOC_CALL void *_malloc_base( size_t nSize )
{
	return AllocUnattributed( nSize );
}
#endif

ALLOC_CALL void *_calloc_base( size_t nCount, size_t nSize )
{
	void *pMem = AllocUnattributed( nSize * nCount );
	if ( pMem )
		memset( pMem, 0, nSize * nCount );
	return pMem;
}

ALLOC_CALL void *_realloc_base( void *pMem, size_t nSize )
{
	return ReallocUnattributed( pMem, nSize );
}

#if ( defined ( _MSC_VER ) && _MSC_VER >= 1920 )
ALLOC_CALL void *_recalloc_base( void *pMem, size_t nCount, size_t nSize )
{
	return RecallocUnattributed( pMem, nSize * nCount );
}
#else
ALLOC_CALL void *_recalloc_base(void *pMem, size_t nSize)
{
	return RecallocUnattributed( pMem, nSize );
}
#endif

void _free_base( void *pMem )
{
	g_pMemAlloc->Free( pMem );
}

void *__cdecl _expand_base( void *pMem, size_t nNewSize, int nBlockUse )
{
	Assert( 0 );
	return NULL;
}

// crt
void * __cdecl _malloc_crt(size_t size)
{
	return AllocUnattributed( size );
}

void * __cdecl _calloc_crt(size_t count, size_t size)
{
	return _calloc_base( count, size );
}

void * __cdecl _realloc_crt(void *ptr, size_t size)
{
	return _realloc_base( ptr, size );
}

void * __cdecl _recalloc_crt(void *ptr, size_t count, size_t size)
{
#if ( defined ( _MSC_VER ) && _MSC_VER >= 1920 )
	return _recalloc_base( ptr, count, size );
#else
	return _recalloc_base( ptr, size * count );
#endif
}

ALLOC_CALL void * __cdecl _recalloc ( void * memblock, size_t count, size_t size )
{
	return RecallocUnattributed( memblock, size * count );
}

size_t _msize_base( void *pMem ) _CRT_NOEXCEPT
{
	return g_pMemAlloc->GetSize(pMem);
}

size_t _msize( void *pMem )
{
	return _msize_base(pMem);
}

size_t msize( void *pMem )
{
	return g_pMemAlloc->GetSize(pMem);
}

void *__cdecl _heap_alloc( size_t nSize )
{
	return AllocUnattributed( nSize );
}

void *__cdecl _nh_malloc( size_t nSize, int )
{
	return AllocUnattributed( nSize );
}

void *__cdecl _expand( void *pMem, size_t nSize )
{
	Assert( 0 );
	return NULL;
}

unsigned int _amblksiz = 16; //BYTES_PER_PARA;

#if _MSC_VER >= 1400
HANDLE _crtheap = (HANDLE)1;	// PatM Can't be 0 or CRT pukes
int __active_heap = 1;
#endif //  _MSC_VER >= 1400

size_t __cdecl _get_sbh_threshold( void )
{
	return 0;
}

int __cdecl _set_sbh_threshold( size_t )
{
	return 0;
}

int _heapchk()
{
#ifdef _WIN32
	return _HEAPOK;
#else
	return 1;
#endif
}

int _heapmin()
{
	return 1;
}

int __cdecl _heapadd( void *, size_t )
{
	return 0;
}

int __cdecl _heapset( unsigned int )
{
	return 0;
}

size_t __cdecl _heapused( size_t *, size_t * )
{
	return 0;
}

#ifdef _WIN32
int __cdecl _heapwalk( _HEAPINFO * )
{
	return 0;
}
#endif

} // end extern "C"


//-----------------------------------------------------------------------------
// Debugging functions that we're going to override to call our allocator
// NOTE: These have to be here for release + debug builds in case we
// link to a debug static lib!!!
//-----------------------------------------------------------------------------

extern "C"
{
	
void *malloc_db( size_t nSize, const char *pFileName, int nLine )
{
	return g_pMemAlloc->AllocDbg( nSize );
}

void free_db( void *pMem, const char *pFileName, int nLine )
{
	g_pMemAlloc->FreeDbg( pMem );
}

void *realloc_db( void *pMem, size_t nSize, const char *pFileName, int nLine )
{
	return g_pMemAlloc->ReallocDbg( pMem, nSize );
}
	
} // end extern "C"

//-----------------------------------------------------------------------------
// These methods are standard MSVC heap initialization + shutdown methods
//-----------------------------------------------------------------------------
extern "C"
{

	int __cdecl _heap_init()
	{
		return g_pMemAlloc != NULL;
	}

	void __cdecl _heap_term()
	{
	}

}
#endif


//-----------------------------------------------------------------------------
// Prevents us from using an inappropriate new or delete method,
// ensures they are here even when linking against debug or release static libs
//-----------------------------------------------------------------------------
#ifndef NO_MEMOVERRIDE_NEW_DELETE
void *__cdecl operator new( size_t nSize )
{
	return AllocUnattributed( nSize );
}

void __cdecl operator delete( void *pMem ) noexcept
{
	g_pMemAlloc->Free( pMem );
}

void operator delete(void *pMem, std::size_t) noexcept
{
	g_pMemAlloc->Free( pMem );
}

void *__cdecl operator new( size_t nSize, const std::nothrow_t & ) noexcept
{
	return AllocUnattributed( nSize );
}

void __cdecl operator delete( void *pMem, const std::nothrow_t & ) noexcept
{
	g_pMemAlloc->Free( pMem );
}

void *__cdecl operator new( size_t nSize, std::align_val_t nAlign )
{
	return g_pMemAlloc->RegionAllocAligned( MemAllocRegion::AllocNewAligned, nSize, (size_t)nAlign );
}

void *__cdecl operator new( size_t nSize, std::align_val_t nAlign, const std::nothrow_t & ) noexcept
{
	return g_pMemAlloc->RegionAllocAligned( MemAllocRegion::AllocNewAligned, nSize, (size_t)nAlign );
}

void __cdecl operator delete( void *pMem, std::align_val_t ) noexcept
{
	g_pMemAlloc->RegionFreeAligned( MemAllocRegion::FreeNewAligned, pMem );
}

void __cdecl operator delete( void *pMem, std::size_t, std::align_val_t ) noexcept
{
	g_pMemAlloc->RegionFreeAligned( MemAllocRegion::FreeNewAligned, pMem );
}

void __cdecl operator delete( void *pMem, std::align_val_t, const std::nothrow_t & ) noexcept
{
	g_pMemAlloc->RegionFreeAligned( MemAllocRegion::FreeNewAligned, pMem );
}

void *__cdecl operator new[] ( size_t nSize )
{
	return AllocUnattributed( nSize );
}

void __cdecl operator delete[] ( void *pMem ) noexcept
{
	g_pMemAlloc->Free( pMem );
}

void __cdecl operator delete[] ( void *pMem, std::size_t ) noexcept
{
	g_pMemAlloc->Free( pMem );
}

void *__cdecl operator new[] ( size_t nSize, const std::nothrow_t & ) noexcept
{
	return AllocUnattributed( nSize );
}

void __cdecl operator delete[] ( void *pMem, const std::nothrow_t & ) noexcept
{
	g_pMemAlloc->Free( pMem );
}

void *__cdecl operator new[] ( size_t nSize, std::align_val_t nAlign )
{
	return g_pMemAlloc->RegionAllocAligned( MemAllocRegion::AllocNewArrayAligned, nSize, (size_t)nAlign );
}

void *__cdecl operator new[] ( size_t nSize, std::align_val_t nAlign, const std::nothrow_t & ) noexcept
{
	return g_pMemAlloc->RegionAllocAligned( MemAllocRegion::AllocNewArrayAligned, nSize, (size_t)nAlign );
}

void __cdecl operator delete[] ( void *pMem, std::align_val_t ) noexcept
{
	g_pMemAlloc->RegionFreeAligned( MemAllocRegion::FreeNewArrayAligned, pMem );
}

void __cdecl operator delete[] ( void *pMem, std::size_t, std::align_val_t ) noexcept
{
	g_pMemAlloc->RegionFreeAligned( MemAllocRegion::FreeNewArrayAligned, pMem );
}

void __cdecl operator delete[] ( void *pMem, std::align_val_t, const std::nothrow_t & ) noexcept
{
	g_pMemAlloc->RegionFreeAligned( MemAllocRegion::FreeNewArrayAligned, pMem );
}
#endif


//-----------------------------------------------------------------------------
// Override some debugging allocation methods in MSVC
// NOTE: These have to be here for release + debug builds in case we
// link to a debug static lib!!!
//-----------------------------------------------------------------------------
#ifndef _STATIC_LINKED

extern "C"
{
	
void *__cdecl _nh_malloc_dbg( size_t nSize, int nFlag, int nBlockUse,
								const char *pFileName, int nLine )
{
	return g_pMemAlloc->AllocDbg( nSize );
}

void *__cdecl _malloc_dbg( size_t nSize, int nBlockUse,
							const char *pFileName, int nLine )
{
	return g_pMemAlloc->AllocDbg( nSize );
}

void *__cdecl _calloc_dbg( size_t nNum, size_t nSize, int nBlockUse,
							const char *pFileName, int nLine )
{
	void *pMem = g_pMemAlloc->AllocDbg( nSize * nNum );
	if ( pMem )
		memset( pMem, 0, nSize * nNum );
	return pMem;
}

void *__cdecl _calloc_dbg_impl( size_t nNum, size_t nSize, int nBlockUse,
								const char *szFileName, int nLine, int *errno_tmp )
{
	return _calloc_dbg( nNum, nSize, nBlockUse, szFileName, nLine );
}

void *__cdecl _realloc_dbg( void *pMem, size_t nNewSize, int nBlockUse,
							const char *pFileName, int nLine )
{
	return g_pMemAlloc->ReallocDbg( pMem, nNewSize );
}

void *__cdecl _expand_dbg( void *pMem, size_t nNewSize, int nBlockUse,
							const char *pFileName, int nLine )
{
	Assert( 0 );
	return NULL;
}

void __cdecl _free_dbg( void *pMem, int nBlockUse )
{
	g_pMemAlloc->FreeDbg( pMem );
}

size_t __cdecl _msize_dbg( void *pMem, int nBlockUse )
{
	return g_pMemAlloc->GetSize( pMem );
}


#ifdef _WIN32

#if defined(_DEBUG) && _MSC_VER >= 1300
// aligned base
ALLOC_CALL void *__cdecl _aligned_malloc_base( size_t size, size_t align )
{
	return g_pMemAlloc->AllocAligned( size, align );
}

ALLOC_CALL void *__cdecl _aligned_realloc_base( void *ptr, size_t size, size_t align )
{
	return MemAlloc_ReallocAligned( ptr, size, align );
}

ALLOC_CALL void *__cdecl _aligned_recalloc_base( void *ptr, size_t size, size_t align )
{
	Error( "Unsupported function\n" );
	return NULL;
}

FREE_CALL void __cdecl _aligned_free_base( void *ptr )
{
	MemAlloc_FreeAligned( ptr );
}

// aligned
ALLOC_CALL void * __cdecl _aligned_malloc( size_t size, size_t align )
{
	return _aligned_malloc_base(size, align);
}

ALLOC_CALL void *__cdecl _aligned_realloc(void *memblock, size_t size, size_t align)
{
    return _aligned_realloc_base(memblock, size, align);
}

ALLOC_CALL void * __cdecl _aligned_recalloc( void * memblock, size_t count, size_t size, size_t align )
{
    return _aligned_recalloc_base(memblock, count * size, align);
}

FREE_CALL void __cdecl _aligned_free( void *memblock )
{
    _aligned_free_base(memblock);
}

// aligned offset base
ALLOC_CALL void * __cdecl _aligned_offset_malloc_base( size_t size, size_t align, size_t offset )
{
	return NULL;
}

ALLOC_CALL void * __cdecl _aligned_offset_realloc_base( void * memblock, size_t size, size_t align, size_t offset)
{
	return NULL;
}

ALLOC_CALL void * __cdecl _aligned_offset_recalloc_base( void * memblock, size_t size, size_t align, size_t offset)
{
	return NULL;
}

// aligned offset
ALLOC_CALL void *__cdecl _aligned_offset_malloc(size_t size, size_t align, size_t offset)
{
    return _aligned_offset_malloc_base( size, align, offset );
}

ALLOC_CALL void *__cdecl _aligned_offset_realloc(void *memblock, size_t size, size_t align, size_t offset)
{
    return _aligned_offset_realloc_base( memblock, size, align, offset );
}

ALLOC_CALL void * __cdecl _aligned_offset_recalloc( void * memblock, size_t count, size_t size, size_t align, size_t offset )
{
    return _aligned_offset_recalloc_base( memblock, count * size, align, offset );
}

#endif // _MSC_VER >= 1400

#endif

} // end extern "C"


//-----------------------------------------------------------------------------
// Override some the _CRT debugging allocation methods in MSVC
//-----------------------------------------------------------------------------
#ifdef _WIN32

extern "C"
{
	
int _CrtDumpMemoryLeaks(void)
{
	return 0;
}

_CRT_DUMP_CLIENT _CrtSetDumpClient( _CRT_DUMP_CLIENT dumpClient )
{
	return NULL;
}

int _CrtSetDbgFlag( int nNewFlag )
{
	return 0;
}

// 64-bit port.
#define AFNAME(var) __p_##var
#define AFRET(var)  &var
#if defined(_crtDbgFlag)
#undef _crtDbgFlag
#endif
#if defined(_crtBreakAlloc)
#undef _crtBreakAlloc
#endif

int _crtDbgFlag = _CRTDBG_ALLOC_MEM_DF;
int* AFNAME(_crtDbgFlag)(void)
{
	return AFRET(_crtDbgFlag);
}

long _crtBreakAlloc;      /* Break on this allocation */
long* AFNAME(_crtBreakAlloc) (void)
{
	return AFRET(_crtBreakAlloc);
}

void __cdecl _CrtSetDbgBlockType( void *pMem, int nBlockUse )
{
	DebuggerBreak();
}

_CRT_ALLOC_HOOK __cdecl _CrtSetAllocHook( _CRT_ALLOC_HOOK pfnNewHook )
{
	DebuggerBreak();
	return NULL;
}

long __cdecl _CrtSetBreakAlloc( long lNewBreakAlloc )
{
	return 0;
}
					 
// A live block is MemoryState_UnexpectedlyAllocated. Outside the debug memory mode tier0 only
// rejects NULL and misaligned pointers and reports MemoryState_Operational for the rest
static bool IsValidHeapBlock( const void *pMem )
{
	MemoryState state = g_pMemAlloc->GetMemoryState( const_cast<void *>( pMem ) );
	return state == MemoryState_UnexpectedlyAllocated || state == MemoryState_Operational;
}

int __cdecl _CrtIsValidHeapPointer( const void *pMem )
{
	return IsValidHeapBlock( pMem );
}

int __cdecl _CrtIsValidPointer( const void *pMem, unsigned int size, int access )
{
	return 1;
}

int __cdecl _CrtCheckMemory( void )
{
	return 1;
}

int __cdecl _CrtIsMemoryBlock( const void *pMem, unsigned int nSize,
    long *plRequestNumber, char **ppFileName, int *pnLine )
{
	// There are no allocation request numbers or file and line info to return
	if ( plRequestNumber )
		*plRequestNumber = 0;
	if ( ppFileName )
		*ppFileName = NULL;
	if ( pnLine )
		*pnLine = 0;

	return IsValidHeapBlock( pMem );
}

int __cdecl _CrtMemDifference( _CrtMemState *pState, const _CrtMemState * oldState, const _CrtMemState * newState )
{
	DebuggerBreak();
	return FALSE;
}

void __cdecl _CrtMemDumpStatistics( const _CrtMemState *pState )
{
	DebuggerBreak();	
}

void __cdecl _CrtMemCheckpoint( _CrtMemState *pState )
{
}

void __cdecl _CrtMemDumpAllObjectsSince( const _CrtMemState *pState )
{
	DebuggerBreak();
}

void __cdecl _CrtDoForAllClientObjects( void (*pfn)(void *, void *), void * pContext )
{
	DebuggerBreak();
}


//-----------------------------------------------------------------------------
// Methods in dbgrpt.cpp 
//-----------------------------------------------------------------------------
long _crtAssertBusy = -1;

int __cdecl _CrtSetReportMode( int nReportType, int nReportMode )
{
	return 0;
}

_HFILE __cdecl _CrtSetReportFile( int nRptType, _HFILE hFile )
{
	return NULL;
}

_CRT_REPORT_HOOK __cdecl _CrtSetReportHook( _CRT_REPORT_HOOK pfnNewHook )
{
	return NULL;
}

int __cdecl _CrtDbgReport( int nRptType, const char * szFile,
        int nLine, const char * szModule, const char * szFormat, ... )
{
	return 0;
}

#if _MSC_VER >= 1400

#if defined( _DEBUG ) && _MSC_VER < 1900
 
// wrapper which passes no debug info; not available in debug
void __cdecl _invalid_parameter_noinfo(void)
{
    Assert(0);
}

#endif /* defined( _DEBUG ) */

#if defined( _DEBUG )

int __cdecl __crtMessageWindowW( int nRptType, const wchar_t * szFile, const wchar_t * szLine,
								 const wchar_t * szModule, const wchar_t * szUserMessage )
{
	Assert(0);
	return 0;
}

int __cdecl _CrtDbgReportV( int nRptType, const wchar_t *szFile, int nLine, 
						    const wchar_t *szModule, const wchar_t *szFormat, va_list arglist )
{
	Assert(0);
	return 0;
}

int __cdecl _CrtDbgReportW( int nRptType, const wchar_t *szFile, int nLine, 
						    const wchar_t *szModule, const wchar_t *szFormat, ...)
{
	Assert(0);
	return 0;
}

#if ( defined(_MSC_VER) && _MSC_VER >= 1900)
int __cdecl _VCrtDbgReportA(int nRptType, void *pReturnAddr, const char *szFile, int nLine,
							const char *szModule, const char *szFormat, va_list arglist)
{
	Assert(0);
	return 0;
}
#else
int __cdecl _VCrtDbgReportA(int nRptType, const wchar_t *szFile, int nLine,
							const wchar_t *szModule, const wchar_t *szFormat, va_list arglist)
{
	Assert(0);
	return 0;
}
#endif

int __cdecl _CrtSetReportHook2( int mode, _CRT_REPORT_HOOK pfnNewHook )
{
	_CrtSetReportHook( pfnNewHook );
	return 0;
}
 

#endif  /* defined( _DEBUG ) */

extern "C" int __crtDebugCheckCount = FALSE;

extern "C" int __cdecl _CrtSetCheckCount( int fCheckCount )
{
    int oldCheckCount = __crtDebugCheckCount;
    return oldCheckCount;
}

extern "C" int __cdecl _CrtGetCheckCount( void )
{
    return __crtDebugCheckCount;
}

// aligned offset debug
extern "C" void * __cdecl _aligned_offset_recalloc_dbg( void * memblock, size_t count, size_t size, size_t align, size_t offset, const char * f_name, int line_n )
{
	return RecallocUnattributed( memblock, size * count );
}

extern "C" void * __cdecl _aligned_recalloc_dbg( void *memblock, size_t count, size_t size, size_t align, const char * f_name, int line_n )
{
    return _aligned_offset_recalloc_dbg(memblock, count, size, align, 0, f_name, line_n);
}

extern "C" void * __cdecl _recalloc_dbg ( void * memblock, size_t count, size_t size, int nBlockUse, const char * szFileName, int nLine )
{
	return _aligned_offset_recalloc_dbg(memblock, count, size, 0, 0, szFileName, nLine);
}

_CRT_REPORT_HOOK __cdecl _CrtGetReportHook( void )
{
	return NULL;
}

#endif
int __cdecl _CrtReportBlockType(const void * pUserData)
{
	return 0;
}


} // end extern "C"
#endif // _WIN32

// Most files include this file, so when it's used it adds an extra .ValveDbg section,
// to help identify debug binaries.
#ifdef _WIN32
	#ifndef NDEBUG // _DEBUG
		#pragma data_seg("ValveDBG") 
		volatile const char* DBG = "*** DEBUG STUB ***";                     
	#endif
#endif

#endif

// Extras added prevent dbgheap.obj from being included - DAL
#ifdef _WIN32

extern "C"
{
size_t __crtDebugFillThreshold = 0;

extern "C" void * __cdecl _heap_alloc_base (size_t size) 
{
    Assert(0);
	return NULL;
}


void * __cdecl _heap_alloc_dbg( size_t nSize, int nBlockUse, const char * szFileName, int nLine)
{
		return _heap_alloc(nSize);
}

// 64-bit
#ifdef _WIN64
static void * __cdecl realloc_help( void * pUserData, size_t * pnNewSize, int nBlockUse,const char * szFileName,
				int nLine, int fRealloc )
{
		Assert(0); // Shouldn't be needed
		return NULL;
}
#else
static void * __cdecl realloc_help( void * pUserData, size_t nNewSize, int nBlockUse, const char * szFileName,
                  int nLine, int fRealloc)
{
		Assert(0); // Shouldn't be needed
		return NULL;
}
#endif

void __cdecl _free_nolock( void * pUserData)
{
		// I don't think the second param is used in memoverride
        _free_dbg(pUserData, 0);
}

void __cdecl _free_dbg_nolock( void * pUserData, int nBlockUse)
{
        _free_dbg(pUserData, 0);
}

_CRT_ALLOC_HOOK __cdecl _CrtGetAllocHook ( void)
{
		Assert(0); 
        return NULL;
}

static int __cdecl CheckBytes( unsigned char * pb, unsigned char bCheck, size_t nSize)
{
        int bOkay = TRUE;
        return bOkay;
}


_CRT_DUMP_CLIENT __cdecl _CrtGetDumpClient ( void)
{
		Assert(0); 
        return NULL;
}

#if _MSC_VER >= 1400
static void __cdecl _printMemBlockData( _locale_t plocinfo, _CrtMemBlockHeader * pHead)
{
}

static void __cdecl _CrtMemDumpAllObjectsSince_stat( const _CrtMemState * state, _locale_t plocinfo)
{
}
#endif
void * __cdecl _aligned_malloc_dbg( size_t size, size_t align, const char * f_name, int line_n)
{
    return _aligned_malloc(size, align);
}

void * __cdecl _aligned_realloc_dbg( void *memblock, size_t size, size_t align,
               const char * f_name, int line_n)
{
    return _aligned_realloc(memblock, size, align);
}

void * __cdecl _aligned_offset_malloc_dbg( size_t size, size_t align, size_t offset,
              const char * f_name, int line_n)
{
    return _aligned_offset_malloc(size, align, offset);
}

void * __cdecl _aligned_offset_realloc_dbg( void * memblock, size_t size, size_t align, 
                 size_t offset, const char * f_name, int line_n)
{
    return _aligned_offset_realloc(memblock, size, align, offset);
}

void __cdecl _aligned_free_dbg( void * memblock)
{
    _aligned_free(memblock);
}

#if defined( _MSC_VER ) && _MSC_VER < 1900
size_t __cdecl _CrtSetDebugFillThreshold( size_t _NewDebugFillThreshold)
{
	Assert(0);
    return 0;
}

//===========================================
// NEW!!! 64-bit

char * __cdecl _strdup ( const char * string )
{
	int nSize = (int)strlen(string) + 1;
	// Check for integer underflow.
	if(nSize <= 0)
		return NULL;
	char *pCopy = (char*)AllocUnattributed( nSize );
	if ( pCopy )
		memcpy( pCopy, string, nSize );
	return pCopy;
}
#endif

#if 0
_TSCHAR * __cdecl _tfullpath_dbg ( _TSCHAR *UserBuf, const _TSCHAR *path, size_t maxlen, int nBlockUse, const char * szFileName, int nLine )
{
	Assert(0);
	return NULL;
}

_TSCHAR * __cdecl _tfullpath ( _TSCHAR *UserBuf, const _TSCHAR *path, size_t maxlen )
{
	Assert(0);
	return NULL;
}

_TSCHAR * __cdecl _tgetdcwd_lk_dbg ( int drive, _TSCHAR *pnbuf, int maxlen, int nBlockUse, const char * szFileName, int nLine )
{
	Assert(0);
	return NULL;
}

_TSCHAR * __cdecl _tgetdcwd_nolock ( int drive, _TSCHAR *pnbuf, int maxlen )
{
	Assert(0);
	return NULL;
}

errno_t __cdecl _tdupenv_s_helper ( _TSCHAR **pBuffer, size_t *pBufferSizeInTChars, const _TSCHAR *varname, int nBlockUse, const char * szFileName, int nLine )
{
	Assert(0);
	return 0;
}

errno_t __cdecl _tdupenv_s_helper ( _TSCHAR **pBuffer, size_t *pBufferSizeInTChars, const _TSCHAR *varname )
{
	Assert(0);
	return 0;
}

_TSCHAR * __cdecl _ttempnam_dbg ( const _TSCHAR *dir, const _TSCHAR *pfx, int nBlockUse, const char * szFileName, int nLine )
{
	Assert(0);
	return 0;
}

_TSCHAR * __cdecl _ttempnam ( const _TSCHAR *dir, const _TSCHAR *pfx )
{
	Assert(0);
	return 0;
}

wchar_t * __cdecl _wcsdup_dbg ( const wchar_t * string, int nBlockUse, const char * szFileName, int nLine )
{
	Assert(0);
	return 0;
}

wchar_t * __cdecl _wcsdup ( const wchar_t * string )
{
	Assert(0);
	return 0;
}
#endif

} // end extern "C"

#if _MSC_VER >= 1400

#pragma warning(push)
#pragma warning(disable: 4483)
#if _MSC_FULL_VER >= 140050415
#define _NATIVE_STARTUP_NAMESPACE  __identifier("<CrtImplementationDetails>")
#else  /* _MSC_FULL_VER >= 140050415 */
#define _NATIVE_STARTUP_NAMESPACE __CrtImplementationDetails
#endif  /* _MSC_FULL_VER >= 140050415 */

namespace _NATIVE_STARTUP_NAMESPACE
{
    class NativeDll
    {
    private:
        static const unsigned int ProcessDetach   = 0;
        static const unsigned int ProcessAttach   = 1;
        static const unsigned int ThreadAttach    = 2;
        static const unsigned int ThreadDetach    = 3;
        static const unsigned int ProcessVerifier = 4;

    public:

        inline static bool IsInDllMain()
        {
            return false;
        }

        inline static bool IsInProcessAttach()
        {
            return false;
        }

        inline static bool IsInProcessDetach()
        {
            return false;
        }

        inline static bool IsInVcclrit()
        {
            return false;
        }

        inline static bool IsSafeForManagedCode()
        {
            if (!IsInDllMain())
            {
                return true;
            }

            if (IsInVcclrit())
            {
                return true;
            }

            return !IsInProcessAttach() && !IsInProcessDetach();
        }
    };
}
#pragma warning(pop)

#endif // _MSC_VER >= 1400

#endif // !STEAM && !NO_MALLOC_OVERRIDE

#endif // _WIN32
