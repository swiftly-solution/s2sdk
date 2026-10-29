#ifndef TIER0_MEMALLOC_H
#define TIER0_MEMALLOC_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
class CBufferString;

enum MemAllocRegion
{
    AllocDefault = '4',
    FreeDefault,
    AllocAligned,
    FreeAligned,
    AllocHeapAligned,
    FreeHeapAligned,
    AllocKV3 = ':',
    FreeKV3 =  ';',
    AllocNewAligned = '<',
    FreeNewAligned = '=',
    AllocUnk6 = '>',
    FreeUnk6 = '?',
    AllocNewArrayAligned = '@',
    FreeNewArrayAligned = 'A' 
};

enum MemAllocAttribute_t
{
	MemAllocAttribute_Unk0 = 0,
	MemAllocAttribute_Unk1 = 1,
	MemAllocAttribute_Unk2 = 2
};

enum MemoryState
{
	MemoryState_UnexpectedlyAllocated = 0,
	MemoryState_UnexpectedlyFreed = 1,
	MemoryState_UnexpectedlyUnrecognized = 2,
	MemoryState_Corrupt = 3,
	MemoryState_Invalid = 4,
	MemoryState_Operational = 5,
	MemoryState_Unknown = 6
};

enum MemoryDebugType : int16_t
{
	MemoryDebugType_None = 0,
	MemoryDebugType_Light = 1,
	MemoryDebugType_Full = 2,
	MemoryDebugType_Unk3 = 3,
	MemoryDebugType_Unk4 = 4
};

struct MemoryInfoState
{
	MemoryInfoState( MemoryDebugType type, bool debug = false );

	MemoryDebugType m_MemType;
	bool m_bDebug;
};

PLATFORM_INTERFACE void CMemAllocSystemInitialize();

class IMemAlloc
{
	// AMNOTE: A lot of functions in here might be stubbed out and not do what their description tells
	// this heavily depends on the allocator implementation and should be taken into account when using this directly!

private:
	virtual ~IMemAlloc() = 0;

public:
	// Release versions
	virtual void *Alloc( size_t nSize ) = 0;
	virtual void *Realloc( void *pMem, size_t nSize ) = 0;
	virtual void Free( void *pMem ) = 0;

	virtual void *AllocAligned( size_t nSize, size_t align ) = 0;
	virtual void *ReallocAligned( void *pMem, size_t nSize, size_t align ) = 0;
	virtual void FreeAligned( void *pMem ) = 0;

	// AMNOTE: It's unclear if the functions below are actually a debug variants, as in binaries they are
	// absolutely the same to the above functions, but with a different name as they aren't merged together
	// in the vtable in the win binaries. So it's mostly a guess, same goes to their params.
	// ===============================================================
	virtual void *AllocDbg( size_t nSize ) = 0;
	virtual void *ReallocDbg( void *pMem, size_t nSize ) = 0;
	virtual void FreeDbg( void *pMem ) = 0;

	virtual void *AllocAlignedDbg( size_t nSize, size_t align ) = 0;
	virtual void *ReallocAlignedDbg( void *pMem, size_t nSize, size_t align ) = 0;
	virtual void FreeAlignedDbg( void *pMem ) = 0;
	// ===============================================================

	// Region-based allocations
	// Use MEMALLOC_REGION_* defines for the region arg
	// AMNOTE: Region name is mostly a guess!
	virtual void *RegionAlloc( MemAllocRegion region, size_t nSize ) = 0;
	virtual void RegionFree( MemAllocRegion region, void *pMem ) = 0;

	virtual void *RegionAllocAligned( MemAllocRegion region, size_t nSize, size_t align ) = 0;
	virtual void RegionFreeAligned( MemAllocRegion region, void *pMem ) = 0;

	// Returns size of a particular allocation
	virtual size_t GetSize( void *pMem ) = 0;
	virtual size_t GetSizeAligned( void *pMem ) = 0;

	// Writes the stats to "<out_path>.txt" next to the executable, or to "memstats_<n>.txt" when out_path is NULL,
	// "<stdout>" prints them to the console instead. Does nothing with the default pass-through allocator
	virtual void DumpStats( const char *out_path ) = 0;

	// AMNOTE: Stub, returns -1
	virtual int unk001() = 0;

	// AMNOTE: Stub
	virtual void unk002() = 0;

	// AMNOTE: Used by the CMemoryStack::Init
	virtual void *LockAllocStatsForCurrentStack( uint32_t &allocid, bool ) = 0;
	virtual void *LockAllocStats( uint32_t allocid ) = 0;
	virtual void UnlockAllocStats( uint32_t allocid ) = 0;

	virtual void CompactOnFail() = 0;

	// Logs the out of memory message, breaks in the debugger and exits the process
	virtual void ReportFailedAllocation( size_t nSize ) = 0;

	// memset's at the pMem location with nSize bytes of specified type, where:
	// MemoryDebugType_Light - would memset with 0xDD bytes;
	// MemoryDebugType_Full - would memset with 0xD8 bytes;
	// MemoryDebugType_Unk3 - would memset with 0xD4 bytes;
	// the input pMem is returned
	//
	// NOTE: This would do nothing unless state has m_bDebug set or the allocator is in the debug memory mode
	virtual void *MemSetDbg( void *pMem, size_t nSize, MemoryInfoState state ) = 0;

	// Returns true if the underlying allocator is using DebugMemoryType different to DebugMemoryType::DebugMemoryType_None
	// would also return true if the state argument has m_bDebug set as true
	virtual bool IsInDebugMode( MemoryInfoState state ) = 0;

	// If memory is not in the debug mode, returns either MemoryState_Invalid or MemoryState_Operational
	// Otherwise a more deep check would be performed
	virtual MemoryState GetMemoryState( void *pMem ) = 0;

	// Logs a warning and breaks in a debugger if the pMem state doesn't match provided state,
	// MemoryState_Unknown matches any state, but MemoryState_Corrupt and MemoryState_Invalid are
	// always reported. Returns the state pMem is in
	virtual MemoryState ReportBadMemory( void *pMem, MemoryState state = MemoryState_Unknown ) = 0;

	// Returns memory debug type of this allocator
	virtual MemoryDebugType GetMemoryDebugType() = 0;

	// Returns previous total allocation size that was used
	// AMNOTE: Likely unused, the allocators only store the value
	virtual size_t SetTotalAllocationSize( size_t new_total_alloc_size ) = 0;

	// Returns previous allocation limit size that was used
	// AMNOTE: Likely unused, the allocators only store the value
	virtual size_t SetAllocationLimitSize( size_t new_alloc_limit_size ) = 0;

	// Writes detailed info about this allocator and settings used
	// Example output of a non debug allocator:
	// Heap: standard allocator pass-through to low-level
	// Low-level allocator: mimalloc
	//
	// Example output of a debug allocator with custom settings:
	// Heap: standard allocator + mem init + stackstats + light verifier + full mem debug
	// Low-level allocator: mimalloc
	virtual void GetAllocatorDescription( CBufferString &buf ) = 0;

	// Returns true if stackstats is enabled and -memstackstats_disable_pools launch option was used
	virtual bool IsStackStatsPoolsDisabled() = 0;

	// Returns true if stackstats is enabled for this allocator
	virtual bool IsStackStatsEnabled() = 0;

	virtual int GetAllocAttributes( void *pMem ) = 0;
	virtual void SetAllocAttributes( void *pMem, MemAllocAttribute_t alloc_attribute, int unk ) = 0;

	// AMNOTE: Copies data to an unknown struct of byte size 56
	// Returns true if data was written, false otherwise
	virtual bool unk101( void *out_val ) = 0;

	// Calls the lower-level allocator functions directly
	virtual void *AllocRaw( size_t nSize ) = 0;
	virtual void *ReallocRaw( void *pMem, size_t nSize ) = 0;
	virtual void FreeRaw( void *pMem ) = 0;
	virtual size_t GetSizeRaw( void *pMem ) = 0;
};

PLATFORM_INTERFACE IMemAlloc *g_pMemAlloc;
PLATFORM_INTERFACE void *MemAlloc_AllocFunc( size_t nSize );
PLATFORM_INTERFACE void *MemAlloc_CallocFunc( size_t nCount, size_t nSize );
PLATFORM_INTERFACE void *MemAlloc_ReallocFunc( void *pMem, size_t nSize );
PLATFORM_INTERFACE void MemAlloc_FreeFunc( void *pMem );
PLATFORM_INTERFACE void *MemAlloc_AllocAlignedFunc( size_t nSize, size_t align );
PLATFORM_INTERFACE void MemAlloc_FreeAlignedFunc( void *pMem );
PLATFORM_INTERFACE size_t MemAlloc_GetSizeFunc( void *pMem );
PLATFORM_INTERFACE int MemAlloc_GetAllocAttributesFunc( void *pMem );
PLATFORM_INTERFACE void MemAlloc_SetAllocAttributesFunc( void *pMem, MemAllocAttribute_t alloc_attribute, int unk );
PLATFORM_INTERFACE char *MemAlloc_StrDupFunc( const char *pString );
PLATFORM_INTERFACE char *MemAlloc_StrDupWrapperFunc( const char *pString );
PLATFORM_INTERFACE wchar_t *MemAlloc_WcStrDupFunc( const wchar_t *pString );

void *MemAlloc_Alloc( size_t nSize );
void MemAlloc_Free( void *ptr );
size_t MemAlloc_GetSize(void* ptr);
void *MemAlloc_AllocAlignedUnattributed( size_t size, size_t align );
void *MemAlloc_ReallocAligned( void *ptr, size_t size, size_t align );
void MemAlloc_FreeAligned( void *pMemBlock );
size_t MemAlloc_GetSizeAligned( void *pMemBlock );

#define MemAlloc_AllocAligned( s, a )	MemAlloc_AllocAlignedUnattributed( s, a )

#ifndef MEM_ALLOC_CREDIT_CLASS
#define MEM_ALLOC_CREDIT_CLASS()
#endif

#endif