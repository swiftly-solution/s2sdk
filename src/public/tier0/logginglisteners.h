#ifndef LOGGINGLISTENERS_H
#define LOGGINGLISTENERS_H

#ifdef _WIN32
#pragma once
#endif

#include "loggingtypes.h"
#include "wchartypes.h"
#include <stdio.h>

// Used by CColorizedLoggingListener
#include "win32consoleio.h"

//-----------------------------------------------------------------------------
// A basic logging listener which prints to stdout and the debug channel.
//-----------------------------------------------------------------------------
class CSimpleLoggingListener : public ILoggingListener
{
public:
	CSimpleLoggingListener( bool bQuietPrintf = false, bool bQuietDebugger = false );

	  virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage );

	  // If set to true, does not print anything to stdout.
	  bool m_bQuietPrintf;
	  // If set to true, does not print anything to debugger.
	  bool m_bQuietDebugger;
};

//-----------------------------------------------------------------------------
// A basic logging listener for GUI applications
//-----------------------------------------------------------------------------
class CSimpleWindowsLoggingListener : public ILoggingListener
{
public:
	virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage );
};


//-----------------------------------------------------------------------------
// ** NOTE FOR INTEGRATION **
// This was copied over from source 2 rather than integrated because 
// source 2 has more significantly refactored tier0 logging.
//
// A logging listener with Win32 console API color support which which prints 
// to stdout and the debug channel.
//-----------------------------------------------------------------------------
class CColorizedLoggingListener : public CSimpleLoggingListener
{
public:
	CColorizedLoggingListener( bool bQuietPrintf = false, bool bQuietDebugger = false );

	virtual void Log( const LoggingContext_t *pContext, const tchar *pMessage );

	Win32ConsoleColorContext_t m_ColorContext;
};

#endif // LOGGINGLISTENERS_H
