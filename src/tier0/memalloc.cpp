#include "public/tier0/memalloc.h"

void *MemAlloc_Alloc( size_t nSize )
{
    return g_pMemAlloc->Alloc(nSize);
}

void MemAlloc_Free( void *ptr )
{
    g_pMemAlloc->Free(ptr);
}

size_t MemAlloc_GetSize(void* ptr)
{
    return g_pMemAlloc->GetSize(ptr);
}

void *MemAlloc_AllocAlignedUnattributed( size_t size, size_t align )
{
    return g_pMemAlloc->AllocAligned(size, align);
}

void *MemAlloc_ReallocAligned( void *ptr, size_t size, size_t align )
{
    return g_pMemAlloc->ReallocAligned(ptr, size, align);
}

void MemAlloc_FreeAligned( void *pMemBlock )
{
    return g_pMemAlloc->FreeAligned(pMemBlock);
}

size_t MemAlloc_GetSizeAligned( void *pMemBlock )
{
    return g_pMemAlloc->GetSizeAligned(pMemBlock);
}