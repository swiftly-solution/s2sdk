#include "public/tier0/loggingresponsepolicies.h"

LoggingResponse_t CDefaultLoggingResponsePolicy:: OnLog( const LoggingContext_t *pContext )
{
	if ( pContext->m_Severity == LS_ASSERT && !CommandLine()->FindParm( "-noassert" ) ) 
	{
		return LR_DEBUGGER;
	}
	else if ( pContext->m_Severity == LS_ERROR )
	{
		return LR_ABORT;
	}
	else
	{
		return LR_CONTINUE;
	}
}

LoggingResponse_t CNonFatalLoggingResponsePolicy:: OnLog( const LoggingContext_t *pContext )
{
	if ( ( pContext->m_Severity == LS_ASSERT && !CommandLine()->FindParm( "-noassert" ) ) || pContext->m_Severity == LS_ERROR )
	{
		return LR_DEBUGGER;
	}
	else
	{
		return LR_CONTINUE;
	}
}
