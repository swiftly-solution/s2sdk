#ifndef GLOBALSYMBOL_H
#define GLOBALSYMBOL_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"

// Case-insensitive, nHash is the CUtlStringToken hash of the string
PLATFORM_INTERFACE const char *_FindGlobalSymbolByHash( uint32 nHash );
// Case-insensitive
PLATFORM_INTERFACE const char *_FindGlobalSymbol( const char *pString );
// Case-insensitive
PLATFORM_INTERFACE const char *_MakeGlobalSymbol( const char *pString );
// Case-sensitive, in a table of its own
PLATFORM_INTERFACE const char *_MakeGlobalSymbolCaseSensitive( const char *pString );

// A string interned in tier0's case-insensitive global symbol table, so equal strings share a pointer.
class CGlobalSymbol
{
public:
	CGlobalSymbol();

	// User-provided like the engine's, so a CGlobalSymbol argument is passed by address
	CGlobalSymbol( const CGlobalSymbol &other );
	CGlobalSymbol &operator=( const CGlobalSymbol &other ) = default;

	bool operator==( const CGlobalSymbol &other ) const;
	bool operator!=( const CGlobalSymbol &other ) const;

	bool IsValid() const;
	const char *String() const;

private:
	explicit CGlobalSymbol( const char *pString );

	friend CGlobalSymbol FindGlobalSymbolByHash( uint32 nHash );
	friend CGlobalSymbol FindGlobalSymbol( const char *pString );
	friend CGlobalSymbol MakeGlobalSymbol( const char *pString );

	const char *m_pString;
};

CGlobalSymbol FindGlobalSymbolByHash( uint32 nHash );
CGlobalSymbol FindGlobalSymbol( const char *pString );
CGlobalSymbol MakeGlobalSymbol( const char *pString );

// A string interned in tier0's case-sensitive global symbol table, not interchangeable with CGlobalSymbol.
class CGlobalSymbolCaseSensitive
{
public:
	CGlobalSymbolCaseSensitive();

	// AMNOTE: Assumed to match CGlobalSymbol, no engine function passes this type by value
	CGlobalSymbolCaseSensitive( const CGlobalSymbolCaseSensitive &other );
	CGlobalSymbolCaseSensitive &operator=( const CGlobalSymbolCaseSensitive &other ) = default;

	bool operator==( const CGlobalSymbolCaseSensitive &other ) const;
	bool operator!=( const CGlobalSymbolCaseSensitive &other ) const;

	bool IsValid() const;
	const char *String() const;

private:
	explicit CGlobalSymbolCaseSensitive( const char *pString );

	friend CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *pString );

	const char *m_pString;
};

CGlobalSymbolCaseSensitive MakeGlobalSymbolCaseSensitive( const char *pString );

#endif // GLOBALSYMBOL_H
