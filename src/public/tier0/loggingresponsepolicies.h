#ifndef LOGGINGRESPONSEPOLICIES_H
#define LOGGINGRESPONSEPOLICIES_H

#ifdef _WIN32
#pragma once
#endif

#include "loggingtypes.h"
#include "icommandline.h"

//-----------------------------------------------------------------------------
// Default logging response policy used when one is not specified.
//-----------------------------------------------------------------------------
class CDefaultLoggingResponsePolicy : public ILoggingResponsePolicy
{
public:
	virtual LoggingResponse_t OnLog( const LoggingContext_t *pContext );
};

//-----------------------------------------------------------------------------
// A logging response policy which never terminates the process, even on error.
//-----------------------------------------------------------------------------
class CNonFatalLoggingResponsePolicy : public ILoggingResponsePolicy
{
public:
	virtual LoggingResponse_t OnLog( const LoggingContext_t *pContext );
};

#endif // LOGGINGRESPONSEPOLICIES_H
