#ifndef UTLSTRINGLIST_H
#define UTLSTRINGLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "strtools.h"
#include "utlvector.h"

// easy string list class with dynamically allocated strings. For use with V_SplitString, etc.
// Frees the dynamic strings in destructor.
class CUtlStringList : public CUtlVectorAutoPurge< char * >
{
public:
	CUtlStringList();

	CUtlStringList( const char *pString, const char *pSeparator );

	CUtlStringList( const char *pString, const char * const *pSeparators, int nSeparators );

	~CUtlStringList();

	void CopyAndAddToTail( char const *pString );

	static int __cdecl SortFunc( char *const *sz1, char *const *sz2 );

	void PurgeAndDeleteElements();

private:
	DLL_CLASS_IMPORT void SplitString( const char *pString, const char * const *pSeparators, int nSeparators );
};

#endif // UTLSTRINGLIST_H
