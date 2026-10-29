#ifndef FILTERSTRING_H
#define FILTERSTRING_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "utlstring.h"
#include "utlvector.h"

enum FilterStringOp_t
{
	// The string contains the filter: the filter is a '|' separated list of alternatives, each a
	// space separated list of words that must all be found in the string, or not found for words starting with '-'
	FILTER_STRING_OP_CONTAINS = 0,
	// The string is the filter
	FILTER_STRING_OP_EQUALS,
	FILTER_STRING_OP_NOT_CONTAINS,
	FILTER_STRING_OP_NOT_EQUALS,

	// Compare the string's number with the filter's, failing when either isn't a number
	FILTER_STRING_OP_LESS,
	FILTER_STRING_OP_GREATER,
	FILTER_STRING_OP_LESS_OR_EQUAL,
	FILTER_STRING_OP_GREATER_OR_EQUAL,
};

enum FilterStringCase_t
{
	FILTER_STRING_CASE_INSENSITIVE = 0,
	FILTER_STRING_CASE_SENSITIVE,
};

class CFilterString
{
public:
	// The filter is trimmed of whitespace; an empty filter matches every string
	DLL_CLASS_IMPORT CFilterString( const char *pszFilter = "", FilterStringOp_t eOp = FILTER_STRING_OP_CONTAINS, FilterStringCase_t eCase = FILTER_STRING_CASE_INSENSITIVE );
	DLL_CLASS_IMPORT void SetString( const char *pszFilter, FilterStringOp_t eOp = FILTER_STRING_OP_CONTAINS, FilterStringCase_t eCase = FILTER_STRING_CASE_INSENSITIVE );

	// Returns false for a nullptr or empty string unless the filter is empty
	DLL_CLASS_IMPORT bool DoesMatchString( const char *pszString ) const;

	const char *GetString() const;
	FilterStringOp_t GetOp() const;
	FilterStringCase_t GetCase() const;

private:
	struct MatchString_t
	{
		bool m_bExcludeOnMatch;
		CUtlString m_String;
	};

	struct OrGroup_t
	{
		CUtlVector< MatchString_t > m_AndStrings;
	};

	FilterStringOp_t m_Op;
	FilterStringCase_t m_Case;
	CUtlString m_InputString;
	CUtlVector< OrGroup_t > m_OrGroups;

	// The filter as a number, for the comparison ops
	bool m_bIsNumber;
	float m_flNumber;
};

#endif // FILTERSTRING_H
