#if !defined(STEAM) && !defined(NO_MALLOC_OVERRIDE)
#if (defined(_DEBUG) || !defined(_INC_CRTDBG)) || defined(MEMDBGON_H)

#include <string.h>
#include "memalloc.h"

#define MEM_OVERRIDE_ON 1

#undef malloc
#undef realloc
#undef calloc
#undef _expand
#undef free
#undef _msize
#undef _aligned_malloc
#undef _aligned_free

#ifndef MEMDBGON_H
inline void *MemAlloc_InlineCallocMemset( void *pMem, size_t nCount, size_t nElementSize)
{
	memset(pMem, 0, nElementSize * nCount);
	return pMem;
}
#endif

#define calloc(c, s)		MemAlloc_InlineCallocMemset(malloc(c*s), c, s)
#define free(p)				g_pMemAlloc->Free( p )
#define _aligned_free( p )	MemAlloc_FreeAligned( p )
#define _msize(p)			g_pMemAlloc->GetSize( p )
#define _expand(p, s)		_expand_NoLongerSupported(p, s)

#define malloc(s)				MemAlloc_Alloc( s )
#define realloc(p, s)			g_pMemAlloc->Realloc( p, s )
#define _aligned_malloc( s, a )	MemAlloc_AllocAligned( s, a )

#ifndef _malloc_dbg
#define _malloc_dbg(s, t, f, l)	WHYCALLINGTHISDIRECTLY(s)
#endif

#undef new

#undef _strdup
#undef strdup
#undef _wcsdup
#undef wcsdup

#define _strdup(s) MemAlloc_StrDup(s)
#define strdup(s)  MemAlloc_StrDup(s)
#define _wcsdup(s) MemAlloc_WcStrDup(s)
#define wcsdup(s)  MemAlloc_WcStrDup(s)

#if !defined(MEMDBGON_H)

inline char *MemAlloc_StrDup(const char *pString)
{
	char *pMemory;

	if (!pString)
		return NULL;

	size_t len = strlen(pString) + 1;
	if ((pMemory = (char *)malloc(len)) != NULL)
	{
		return strcpy( pMemory, pString );
	}

	return NULL;
}

inline wchar_t *MemAlloc_WcStrDup(const wchar_t *pString)
{
	wchar_t *pMemory;

	if (!pString)
		return NULL;

	size_t len = (wcslen(pString) + 1);
	if ((pMemory = (wchar_t *)malloc(len * sizeof(wchar_t))) != NULL)
	{
		return wcscpy( pMemory, pString );
	}

	return NULL;
}

#define MEMDBGON_H // Defined here so can be used above

#endif

#endif
#endif