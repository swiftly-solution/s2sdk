//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef TIER0_ICOMMANDLINE_H
#define TIER0_ICOMMANDLINE_H
#ifdef _WIN32
#pragma once
#endif

#include "utlstringtoken.h"
#include "bufferstring.h"

class ICommandLine
{
public:
	virtual void		CreateCmdLine( const char *commandline ) = 0;
	virtual void		CreateCmdLine( int argc, char **argv ) = 0;
	virtual void		CreateCmdLinePrependAppName( const char *commandline ) = 0;

    virtual	const char	*CheckParam( CUtlStringToken param, const char **ppszValue = 0 ) const = 0;
	virtual bool		HasParam( CUtlStringToken param ) const = 0;

    // Gets at particular parameters
	virtual int			ParamCount() const = 0;
	virtual int			FindParam( CUtlStringToken param, int startIdx = -1 ) const = 0;	// Returns 0 if not found.
	virtual const char* GetParam( int nIndex ) const = 0;

	// Returns the argument after the one specified, or the default if not found
	virtual const char* ParamValue( CUtlStringToken param, const char *pDefaultVal = 0 ) const = 0;
	virtual int			ParamValue( CUtlStringToken param, int nDefaultVal ) const = 0;
	virtual float		ParamValue( CUtlStringToken param, float flDefaultVal ) const = 0;
	virtual bool		ParamValue( CUtlStringToken param, const char *pDefaultVal, CBufferString *bufOut ) = 0;

	virtual const char **GetParams() const = 0;
	virtual const char *GetCmdLine( void ) const = 0;
	virtual void		AppendParam( const char* param, const char *pszValues ) = 0;
	
	// Returns true if the command line has at least one parm
	virtual bool		IsWellInitialized( void ) const = 0;

	virtual const char *GetParamBuffer() = 0;

	// Unrestricted access to command line parms, alternative methods above won't return anything if
	// they are executed in a secure environment (i.e. wasn't launched with -tools, -dedicated or -insecure)
	virtual	const char *CheckParamUnrestricted( CUtlStringToken param, const char **ppszValue = 0 ) const = 0;
	virtual bool		HasParamUnrestricted( CUtlStringToken param ) const = 0;

	virtual int			FindParamUnrestricted( CUtlStringToken param, int startIdx = -1 ) const = 0;	// Returns 0 if not found.

	// Returns the argument after the one specified, or the default if not found
	virtual const char *ParamValueUnrestricted( CUtlStringToken param, const char *pDefaultVal = 0 ) const = 0;
	virtual int			ParamValueUnrestricted( CUtlStringToken param, int nDefaultVal ) const = 0;
	virtual float		ParamValueUnrestricted( CUtlStringToken param, float flDefaultVal ) const = 0;
	virtual bool		ParamValueUnrestricted( CUtlStringToken param, const char *pDefaultVal, CBufferString *bufOut ) = 0;

	virtual				~ICommandLine() = 0;
};

PLATFORM_INTERFACE ICommandLine *CommandLine();

#endif