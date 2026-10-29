#include "public/tier0/logginglisteners.h"

CSimpleLoggingListener::CSimpleLoggingListener( bool bQuietPrintf, bool bQuietDebugger )
: 
  m_bQuietPrintf( bQuietPrintf ), 
	  m_bQuietDebugger( bQuietDebugger )
{ 
  }

void CSimpleLoggingListener::Log( const LoggingContext_t *pContext, const tchar *pMessage )
{
		  {
#ifndef _CERT
			  if ( !m_bQuietPrintf )
			  {
				  Plat_Printf( _T("%s"), pMessage );
			  }
#endif

#ifdef _WIN32
			  if ( !m_bQuietDebugger && Plat_IsInDebugSession() )
			  {
				  Plat_DebugString( pMessage );
			  }
#endif
		  }
}

void CSimpleWindowsLoggingListener::Log( const LoggingContext_t *pContext, const tchar *pMessage )
{
	if ( Plat_IsInDebugSession() )
	{
		Plat_DebugString( pMessage );
	}
	if ( pContext->m_Severity == LS_ERROR )
	{
		if ( Plat_IsInDebugSession() )
			DebuggerBreak();
		
		if ( !Plat_IsInTestMode() )
			Plat_MessageBox( "Error", pMessage );
	}
}

CColorizedLoggingListener::CColorizedLoggingListener( bool bQuietPrintf, bool bQuietDebugger )
: CSimpleLoggingListener( bQuietPrintf, bQuietDebugger )
{
	InitWin32ConsoleColorContext( &m_ColorContext );
}

void CColorizedLoggingListener::Log( const LoggingContext_t *pContext, const tchar *pMessage )
{
	if ( !m_bQuietPrintf )
	{
		int nPrevColor = -1;

		if ( pContext->m_Color != UNSPECIFIED_LOGGING_COLOR )
		{
			nPrevColor = SetWin32ConsoleColor( &m_ColorContext,
				pContext->m_Color.r(), pContext->m_Color.g(), pContext->m_Color.b(), 
				MAX( MAX( pContext->m_Color.r(), pContext->m_Color.g() ), pContext->m_Color.b() ) > 128 );
		}

		Plat_Printf( _T("%s"), pMessage );

		if ( nPrevColor >= 0 )
		{
			RestoreWin32ConsoleColor( &m_ColorContext, nPrevColor );
		}
	}

#ifdef _WIN32
	if ( !m_bQuietDebugger && Plat_IsInDebugSession() )
	{
		Plat_DebugString( pMessage );
	}
#endif
}
