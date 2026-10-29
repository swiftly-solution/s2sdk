#ifndef STACKCONTEXT_H
#define STACKCONTEXT_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

#ifdef PLATFORM_WINDOWS
#include <xmmintrin.h>
#endif

typedef uintp IntegerRegisterValue_t;

struct MemoryRange_t
{
	bool Contains( const void *p ) const;
	size_t Size() const;

	void *pLow;
	void *pHigh;
};

//-----------------------------------------------------------------------------
// The registers a function has to preserve for its caller
//-----------------------------------------------------------------------------
struct NonVolatileSIMDRegisters_t
{
#ifdef PLATFORM_WINDOWS
	__m128 m_XMM6;
	__m128 m_XMM7;
	__m128 m_XMM8;
	__m128 m_XMM9;
	__m128 m_XMM10;
	__m128 m_XMM11;
	__m128 m_XMM12;
	__m128 m_XMM13;
	__m128 m_XMM14;
	__m128 m_XMM15;
#endif
};

struct _SIMDRegisterAlignmentPadding_t
{
	void *alignmentPadding1;
};

struct NonVolatileIntegerRegisters_t
{
	IntegerRegisterValue_t m_R12;
	IntegerRegisterValue_t m_R13;
	IntegerRegisterValue_t m_R14;
	IntegerRegisterValue_t m_R15;
	IntegerRegisterValue_t m_RBX;
#ifdef PLATFORM_WINDOWS
	IntegerRegisterValue_t m_RSI;
	IntegerRegisterValue_t m_RDI;
#endif
	IntegerRegisterValue_t m_RBP;
};

struct NonVolatileRegisters_t : public NonVolatileSIMDRegisters_t, public _SIMDRegisterAlignmentPadding_t, public NonVolatileIntegerRegisters_t
{
};

struct StackContext_LongJumpLink_t;
struct JmpBuf_AlternateStack_t;

//-----------------------------------------------------------------------------
// Sits at the top of a stack that RunWithAlternateStack runs on, under the frame of the first call
//-----------------------------------------------------------------------------
struct StackContext_StartData_t
{
	IntegerRegisterValue_t m_ParentFramePointer;
	IntegerRegisterValue_t m_ReturnAddress;
#ifdef PLATFORM_WINDOWS
	// The shadow space of the first call
	IntegerRegisterValue_t m_ShadowSpace[4];
#endif
	MemoryRange_t m_StackRange;
	StackContext_LongJumpLink_t *m_pReturnLink;
	StackContext_LongJumpLink_t *m_pExitLink;

	enum ContextFlags_t : uintp
	{
		THREAD_TOP = (1 << 0),
	};

	// ContextFlags_t
	uintp m_nContextFlags;

	DLL_CLASS_IMPORT void UnlinkFromLongJumpHierarchy();
	// Makes the stack return into pJmpBuf when its first call returns
	DLL_CLASS_IMPORT void ReplaceReturnContext( JmpBuf_AlternateStack_t *pJmpBuf );
};

//-----------------------------------------------------------------------------
// Links the stack contexts that a jump leaves and enters
//-----------------------------------------------------------------------------
struct StackContext_LongJumpLink_t
{
	StackContext_StartData_t *m_pExitContext;
	StackContext_StartData_t *m_pEnterContext;

	enum JumpFlags_t : uintp
	{
		// Nothing can jump back out of the entered stack
		UNBREAKABLE = (1 << 0),
	};

	// JumpFlags_t
	uintp m_nJumpFlags;

	JmpBuf_AlternateStack_t *GetEnclosingJmpBuf() const;
};

//-----------------------------------------------------------------------------
// What a jump back to a stack restores, saved by RunWithAlternateStack and JmpToAlternateStack
//-----------------------------------------------------------------------------
struct JmpBuf_AlternateStack_t
{
	StackContext_LongJumpLink_t m_ExitLink;
#ifdef PLATFORM_WINDOWS
	// The stack limits of the thread information block
	MemoryRange_t m_ThreadStackRange;
#endif
	void *alignmentPadding1;
	NonVolatileRegisters_t m_Registers;
};

// m_ExitLink is the first member of JmpBuf_AlternateStack_t

// Calls pFnRun( pPassThroughArg ) on alternateStackRange and returns its result. When ppJmpBack is given it gets the
// JmpBuf of this call, so JmpToAlternateStack can jump back out of the stack and make this return early
PLATFORM_INTERFACE intp RunWithAlternateStack( const MemoryRange_t &alternateStackRange, intp (*pFnRun)( void *pPassThroughArg ), void *pPassThroughArg, JmpBuf_AlternateStack_t **ppJmpBack );
// Jumps to jmpTo, making the call that saved it return nReturnValue. When ppJmpBack is given it gets the JmpBuf of
// this call, so a later jump can come back and make this return
PLATFORM_INTERFACE intp JmpToAlternateStack( JmpBuf_AlternateStack_t &jmpTo, JmpBuf_AlternateStack_t **ppJmpBack, intp nReturnValue );

#ifdef PLATFORM_LINUX
// Makes the first call on pContext's stack return into pJmpBuf, ReplaceReturnContext does it on the other platforms
PLATFORM_INTERFACE void StackContext_SetContextReturn_To_JmpToAlternateStack( StackContext_StartData_t *pContext, JmpBuf_AlternateStack_t *pJmpBuf );
#endif

#endif // STACKCONTEXT_H
