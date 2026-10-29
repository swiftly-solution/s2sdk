//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: setjmp/longjmp based cooperative multitasking system
//
//=============================================================================

#ifndef COROUTINE_H
#define COROUTINE_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

// function prototype
typedef void (*CoroutineFunc_t)( void * );

// handle to a coroutine
typedef int32 HCoroutine;

// creates a new coroutine
// no coroutine code is executed until Coroutine_Continue() is called
PLATFORM_INTERFACE HCoroutine Coroutine_Create( CoroutineFunc_t pFunc, void *pvParam );

// continues the specified coroutine
// returns true if the coroutine is still running, false otherwise
PLATFORM_INTERFACE bool Coroutine_Continue( HCoroutine hCoroutine, const char *pchName = nullptr );

// cancels a currently running coroutine
PLATFORM_INTERFACE void Coroutine_Cancel( HCoroutine hCoroutine );

// called from the coroutine to return control to the main thread
PLATFORM_INTERFACE void Coroutine_YieldToMain();

// returns true if the code is currently running inside of a coroutine
PLATFORM_INTERFACE bool Coroutine_IsActive();

#ifdef PLATFORM_LINUX
// The assembly the coroutines run on. Coroutine_Launch_ASM saves the registers, puts the stack pointer of its caller in
// *ppStackHigh and the address of the saved registers in *ppSavedRegisters, calls pFunc( pvParam ) and then pfnPostLaunch
PLATFORM_INTERFACE void Coroutine_Launch_ASM( void **ppStackHigh, void **ppSavedRegisters, CoroutineFunc_t pFunc, void *pvParam, void (*pfnPostLaunch)() );
// Saves RBX, RBP and R12 to R15 to pRegisters, in that order
PLATFORM_INTERFACE void SaveNonVolatileRegs( uintp *pRegisters );
#endif

#endif // COROUTINE_H
