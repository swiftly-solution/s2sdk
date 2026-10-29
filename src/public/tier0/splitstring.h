#ifndef SPLITSTRING_H
#define SPLITSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "utlvector.h"

class CSplitString : public CUtlVector< char * >
{
public:
	// Splits the string based on separator provided
	// Example: string "test;string string2;here" with a separator ";"
	// results in [ "test", "string string2", "here" ] array entries.
	// Separator can be multicharacter, i.e. "-;" would split string like
	// "test-;string" into [ "test", "string" ]
	CSplitString( const char *pString, const char *pSeparator, bool include_empty = false );

	// Splits the string based on array of separators provided
	// Example: string "test;string,string2;here" with a separators [ ";", "," ]
	// results in [ "test", "string", "string2", "here" ] array entries.
	// Separator can be multicharacter, i.e. "-;" would split string like
	// "test-;string" into [ "test", "string" ]
	CSplitString( const char *pString, const char **pSeparators, int nSeparators, bool include_empty = false );

	~CSplitString();

	// Works the same way as CSplitString::Split with the only exception that it allows you 
	// to provide single string of separators like ";,:" which will then get used to separate string
	// instead of providing an array of separators, but doesn't support multicharacter separators cuz of that!
	DLL_CLASS_IMPORT void SetPacked( const char *pString, const char *pSeparators, bool include_empty = false, int stringSize = -1 );

private:
	DLL_CLASS_IMPORT void Split( const char *pString, int stringSize, const char **pSeparators, int nSeparators, bool include_empty = false );

	void PurgeAndDeleteElements();

private:
	char *m_szBuffer; // a copy of original string, with '\0' instead of separators
	int m_nBufferSize;
};

#endif // SPLITSTRING_H
