//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef MINIDUMP_H
#define MINIDUMP_H
#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "memalloc.h"

// calls the passed in function pointer and catches any exceptions/crashes thrown by it, and writes a minidump
// use from wmain() to protect the whole program
typedef void (*FnWMain)( int , tchar *[] );
typedef int (*FnWMainIntRet)( int , tchar *[] );
typedef void (*FnVoidPtrFn)( void * );

enum ECatchAndWriteMinidumpAction
{
	k_ECatchAndWriteMiniDumpAbort = 0,
	k_ECatchAndWriteMiniDumpReThrow = 1,
	k_ECatchAndWriteMiniDumpIgnore = 2,
};

PLATFORM_INTERFACE void CatchAndWriteMiniDump( FnWMain pfn, int argc, tchar *argv[] ); // action = Abort
PLATFORM_INTERFACE void CatchAndWriteMiniDumpForVoidPtrFn( FnVoidPtrFn pfn, void *pv, bool bExitQuietly ); // action = abort if bExitQuietly, Rethrow otherwise

PLATFORM_INTERFACE void CatchAndWriteMiniDumpEx( FnWMain pfn, int argc, tchar *argv[], ECatchAndWriteMinidumpAction eAction );
PLATFORM_INTERFACE int CatchAndWriteMiniDumpExReturnsInt( FnWMainIntRet pfn, int argc, tchar *argv[], ECatchAndWriteMinidumpAction eAction );
PLATFORM_INTERFACE void CatchAndWriteMiniDumpExForVoidPtrFn( FnVoidPtrFn pfn, void *pv, ECatchAndWriteMinidumpAction eAction );

// Let's not include this.  We'll use forwards instead.
//#include <dbghelp.h>
struct _EXCEPTION_POINTERS;

// AMNOTE: Incomplete, only its start is known
struct MiniDumpStandardData_t
{
	DLL_CLASS_IMPORT void HandlerQueueHeartBeat() const;

	uint32 m_uStructuredExceptionCode;
	// AMNOTE: Nonzero makes MiniDumpExceptionHandler skip its debugger and -nominidumps checks
	int32 m_unk101;
	// Null on Linux
	_EXCEPTION_POINTERS *m_pExceptionInfo;
};

typedef int (*FnMiniDumpHandler)( MiniDumpStandardData_t *pData );

// Returns the previous handler. Linux ignores the handler and returns null.
PLATFORM_INTERFACE FnMiniDumpHandler SetDefaultMiniDumpHandler( FnMiniDumpHandler pfn, bool bRegisterForUnhandledExceptions );
PLATFORM_INTERFACE FnMiniDumpHandler GetDefaultMiniDumpHandler();

class CMiniDumpComment
{
public:
	DLL_CLASS_IMPORT CMiniDumpComment( int nSize, MemAllocAttribute_t allocAttribute );
	DLL_CLASS_IMPORT ~CMiniDumpComment();

	DLL_CLASS_IMPORT char *GetStartPointer();
	DLL_CLASS_IMPORT const char *GetStartPointer() const;
	DLL_CLASS_IMPORT const char *GetEndPointer() const;
	DLL_CLASS_IMPORT char *GetCurrentPointer();
	DLL_CLASS_IMPORT const char *GetCurrentPointer() const;
	DLL_CLASS_IMPORT int GetAvailableBufferSize() const;

	DLL_CLASS_IMPORT void EnsureOSDescription();
	DLL_CLASS_IMPORT void Reset();
	DLL_CLASS_IMPORT void AppendOSComment();
	DLL_CLASS_IMPORT void AppendComment( const char *pszComment );
	DLL_CLASS_IMPORT void PrependComment( const char *pszComment );
	DLL_CLASS_IMPORT void AppendFormattedComment( const char *pszFormat, ... ) FMTFUNCTION( 2, 3 );

	// Appends ch until the comment ends with nCount of them, returns whether it appended any
	DLL_CLASS_IMPORT bool EnsureEndsWithNumCharacters( char ch, int nCount, bool bAppendToEmpty );
	// Returns whether it removed any
	DLL_CLASS_IMPORT bool RemoveTrailingCharacters( char ch );

	DLL_CLASS_IMPORT void OnExceptionCaught();

private:
	int m_nBufferSize;
	MemAllocAttribute_t m_nAllocAttribute;
	char *m_pBuffer;
	char *m_pOSDescription;
	EOSType m_eOSType;
};

#endif // MINIDUMP_H
