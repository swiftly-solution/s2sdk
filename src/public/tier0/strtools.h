#ifndef TIER0_STRTOOLS_H
#define TIER0_STRTOOLS_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"

#ifdef _WIN32
#define CORRECT_PATH_SEPARATOR '\\'
#define CORRECT_PATH_SEPARATOR_S "\\"
#define INCORRECT_PATH_SEPARATOR '/'
#define INCORRECT_PATH_SEPARATOR_S "/"
#else
#define CORRECT_PATH_SEPARATOR '/'
#define CORRECT_PATH_SEPARATOR_S "/"
#define INCORRECT_PATH_SEPARATOR '\\'
#define INCORRECT_PATH_SEPARATOR_S "\\"
#endif

enum EStringConvertErrorPolicy
{
	_STRINGCONVERTFLAG_SKIP =		1,
	_STRINGCONVERTFLAG_FAIL =		2,
	_STRINGCONVERTFLAG_ASSERT =		4,
	_STRINGCONVERTFLAG_UNK001 =		8,

	STRINGCONVERT_REPLACE =			0,
	STRINGCONVERT_SKIP =			_STRINGCONVERTFLAG_SKIP,
	STRINGCONVERT_FAIL =			_STRINGCONVERTFLAG_FAIL,

	STRINGCONVERT_ASSERT_REPLACE =	_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_REPLACE,
	STRINGCONVERT_ASSERT_SKIP =		_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_SKIP,
	STRINGCONVERT_ASSERT_FAIL =		_STRINGCONVERTFLAG_ASSERT + STRINGCONVERT_FAIL,
};

struct characterset_t
{
	char set[256];
};

PLATFORM_INTERFACE int			V_tier0_strlen( const char *str );
PLATFORM_INTERFACE int			V_tier0_strlen16( const uint16_t *str );
PLATFORM_INTERFACE int			V_tier0_strlen32( const uint32_t *str );
PLATFORM_INTERFACE int			V_tier0_wcslen( const wchar_t *str );

PLATFORM_INTERFACE void			V_tier0_strcpy( char *dest, const char *src );
PLATFORM_INTERFACE void			_V_strncpy( char *pDest, const char *pSrc, int maxLen );
PLATFORM_INTERFACE void			V_tier0_strcpy32( uint32_t *dest, const uint32_t *src );
PLATFORM_INTERFACE void			_V_strncpy32_bytes( uint32_t *pDest, const uint32_t *pSrc, int bytes );
PLATFORM_INTERFACE void			V_tier0_wcscpy( wchar_t *dest, const wchar_t *src );

PLATFORM_INTERFACE const char *	V_tier0_strchr( const char *s, char c );
PLATFORM_INTERFACE char *		V_tier0_strrchr( const char *s, char c );
PLATFORM_INTERFACE char *		V_strnchr( const char *s, char c, int n );
PLATFORM_INTERFACE uint32_t *	V_strchr32( const uint32_t *s, uint32_t c );
PLATFORM_INTERFACE wchar_t *	V_tier0_wcschr( const wchar_t *s, wchar_t c );

PLATFORM_INTERFACE int			V_tier0_strcmp( const char *s1, const char *s2 );
PLATFORM_INTERFACE int			_V_strncmp( const char *s1, const char *s2, int n );
PLATFORM_INTERFACE int			V_strcmp32( const uint32_t *s1, const uint32_t *s2 );
PLATFORM_INTERFACE int			V_tier0_wcscmp( const wchar_t *s1, const wchar_t *s2 );

PLATFORM_INTERFACE int			V_stricmp_fast( const char *s1, const char *s2 );

PLATFORM_INTERFACE int			V_stricmp_fast_NegativeForUnequal( const char *s1, const char *s2 );
PLATFORM_INTERFACE int			_V_strnicmp_fast( const char *s1, const char *s2, int n );
PLATFORM_INTERFACE int			V_wcsicmp( const wchar_t *s1, const wchar_t *s2 );
PLATFORM_INTERFACE int			V_wcsnicmp_cch( const wchar_t *s1, const wchar_t *s2, int symbols );

PLATFORM_INTERFACE char *		V_tier0_strstr( const char *s1, const char *search );
PLATFORM_INTERFACE uint32_t *	V_strstr32( const uint32_t *s1, const uint32_t *search );

PLATFORM_INTERFACE char *		V_strupper_fast( char *start );
PLATFORM_INTERFACE uint32_t *	V_towupper32( uint32_t *start );
PLATFORM_INTERFACE wchar_t *	V_towupper( wchar_t *start );

PLATFORM_INTERFACE char *		V_strlower_fast( char *start );
PLATFORM_INTERFACE uint32_t *	V_towlower32( uint32_t *start );
PLATFORM_INTERFACE wchar_t *	V_towlower( wchar_t *start );

PLATFORM_INTERFACE int64_t 		V_atoi64( const char *str );
PLATFORM_INTERFACE uint64_t 		V_atoui64( const char *str );
PLATFORM_INTERFACE double		V_atof( const char *str );
PLATFORM_INTERFACE float		V_atofloat32( const char *str );
PLATFORM_INTERFACE double		V_atofloat64( const char *str );

PLATFORM_INTERFACE double		V_strtod( const char *str, char **endptr = NULL );
PLATFORM_INTERFACE float		V_strtof( const char *str, char **endptr = NULL );
PLATFORM_INTERFACE double		V_wcstod( const wchar_t *str, wchar_t **endptr = NULL );
PLATFORM_INTERFACE int64_t		V_strtoi64( const char *str, char **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE int64_t		V_wcstoi64( const wchar_t *str, wchar_t **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE uint64_t		V_strtoui64( const char *str, char **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE uint64_t		V_wcstoui64( const wchar_t *str, wchar_t **endptr = NULL, int base = 10 );
// Clamp the value to the int32/uint32 range
PLATFORM_INTERFACE int32_t		V_strtoi32( const char *str, char **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE uint32_t		V_strtoui32( const char *str, char **endptr = NULL, int base = 10 );

PLATFORM_OVERLOAD const char *	V_stristr_fast( const char *str, const char *search );
PLATFORM_INTERFACE const char *	_V_strnistr_fast( const char *str, const char *search, int n );
PLATFORM_OVERLOAD const wchar_t *V_wcsistr( const wchar_t *str, const wchar_t *search );

PLATFORM_OVERLOAD int			V_strnlen( const char *str, int n );
PLATFORM_OVERLOAD int			V_strnlen( const uint16_t *str, int n );
PLATFORM_OVERLOAD int			V_strnlen( const uint32_t *str, int n );

PLATFORM_INTERFACE size_t		V_strspn( const char *str, const char *strCharSet );
PLATFORM_INTERFACE int			_V_strcspn( const char *s1, const char *s2 );

inline bool V_isdigit( char c )
{
	return c >= '0' && c <= '9';
}

PLATFORM_INTERFACE bool V_isdigit_str( const char *str );
PLATFORM_INTERFACE int V_iswdigit( uint32_t c );

// These return true for a NULL or empty string
PLATFORM_INTERFACE bool V_isalpha_str_fast( const char *str );
PLATFORM_INTERFACE bool V_isalnum_str_fast( const char *str );
PLATFORM_INTERFACE bool V_isstrlower_fast( const char *pch );
PLATFORM_INTERFACE bool V_isstrupper_fast( const char *pch );
PLATFORM_INTERFACE bool V_isEntirelyWhitespace( const char *pStr );

PLATFORM_INTERFACE bool V_containsWhitespace( const char *pStr );
PLATFORM_INTERFACE bool V_StringHasNonAsciiCharacters( const char *pStr );

// Returns true if V_strtod parses a number at the start of pString
PLATFORM_INTERFACE bool V_AppearsToBeANumber( const char *pString );

// Returns true if the first inputchars characters are hex digits, which must be an even and non-zero count
PLATFORM_INTERFACE bool V_isvalidhex( const char *in, int inputchars );

inline bool V_isempty( const char* pszString ) { return !pszString || !pszString[ 0 ]; }

PLATFORM_INTERFACE int	V_iswspace( uint32_t c );
PLATFORM_INTERFACE int	V_iswalpha( uint32_t c );
PLATFORM_INTERFACE int	V_iswalnum( uint32_t c );
PLATFORM_INTERFACE int	V_iswcntrl( uint32_t c );
PLATFORM_INTERFACE int	V_iswlower( uint32_t c );
PLATFORM_INTERFACE int	V_iswupper( uint32_t c );
PLATFORM_INTERFACE int	V_iswpunct( uint32_t c );
PLATFORM_INTERFACE int	V_isbreakablewspace( wchar_t ch );
PLATFORM_INTERFACE int	V_isbreakablewspace32( uint32_t ch );
PLATFORM_INTERFACE int	V_tier0_ispunct( int c );
PLATFORM_INTERFACE int	V_tier0_isgraph( int c );

#define V_strlen(str)					V_tier0_strlen		((str))
#define V_strlen16(str)					V_tier0_strlen16	((str))
#define V_strlen32(str)					V_tier0_strlen32	((str))
#define V_wcslen(str)					V_tier0_wcslen		((str))

#define V_strcpy(dest, src)				V_tier0_strcpy		((dest), (src))
#define V_strncpy(dest, src, count)		_V_strncpy			((dest), (src), (count))
#define V_strcpy32(dest, src)			V_tier0_strcpy32	((dest), (src))
#define V_strncpy32(dest, src, bytes)	_V_strncpy32_bytes	((dest), (src), (bytes))
#define V_wcscpy(dest, src)				V_tier0_wcscpy		((dest), (src))
#define V_wcsncpy(dest, src, bytes)		_V_wcsncpy_bytes	((dest), (src), (bytes))

#define V_strrchr(s, c)					V_tier0_strrchr		((s), (c))
#define V_wcschr(s, c)					V_tier0_wcschr		((s), (c))

#define V_strcmp(s1, s2)				V_tier0_strcmp		((s1), (s2))
#define V_strncmp(s1, s2, count)		_V_strncmp			((s1), (s2), (count))
#define V_wcscmp(s1, s2)				V_tier0_wcscmp		((s1), (s2))

#define V_stricmp(s1, s2)				V_stricmp_fast		((s1), (s2) )
#define V_stricmp_n(s1, s2)				V_stricmp_fast_NegativeForUnequal((s1), (s2) )
#define V_strnicmp(s1, s2, count)		_V_strnicmp_fast	((s1), (s2), (count))
#define V_wcsnicmp(s1, s2, symbols)		V_wcsnicmp_cch		((s1), (s2), (symbols))
#define V_strcasecmp(s1, s2)			V_stricmp			((s1), (s2))
#define V_strncasecmp(s1, s2, count)	V_strnicmp			((s1), (s2), (count))

#define V_strstr(s1, search)			V_tier0_strstr		((s1), (search))

#define V_strupper(start)				V_strupper_fast		((start))
#define V_strlower(start)				V_strlower_fast		((start))

#define V_stristr(s1, search)			V_stristr_fast		((s1), (search))
#define V_strnistr(s1, search, count)	_V_strnistr_fast	((s1), (search), (count))

#define V_strcspn(s1, s2)				_V_strcspn			((s1), (s2))

#define V_strncat(s1, s2, count)		_V_strncat			((s1), (s2), (count))

#define Q_strlen				V_strlen
#define Q_strcpy				V_strcpy
#define Q_strrchr				V_strrchr
#define Q_strcmp				V_strcmp
#define Q_wcscmp				V_wcscmp
#define Q_stricmp				V_stricmp
#define Q_strstr				V_strstr
#define Q_strupr				V_strupper
#define Q_strlower				V_strlower
#define Q_wcslen				V_wcslen
#define	Q_strncmp				V_strncmp 
#define	Q_strcasecmp			V_stricmp
#define	Q_strncasecmp			V_strnicmp
#define	Q_strnicmp				V_strnicmp
#define	Q_atoi64				V_atoi64
#define Q_atoui64				V_atoui64
#define	Q_atof					V_atof
#define	Q_stristr				V_stristr
#define	Q_strnistr				V_strnistr
#define	Q_strnchr				V_strnchr
#define Q_strncpy				V_strncpy
#define Q_wcsncpy				V_wcsncpy
#define Q_wcsncpy				V_wcsncpy
#define Q_strncat				V_strncat

class DLL_CLASS_IMPORT CUtlCharConversion
{
public:
	struct ConversionArray_t
	{
		char m_nActualChar;
		const char *m_pReplacementString;
	};

	CUtlCharConversion( char nEscapeChar, const char *pDelimiter, int nCount, const ConversionArray_t *pArray );

	CUtlCharConversion( CUtlCharConversion &&rhs );
	CUtlCharConversion( const CUtlCharConversion &rhs );

	CUtlCharConversion &operator=( CUtlCharConversion &&rhs );
	CUtlCharConversion &operator=( const CUtlCharConversion &rhs );

	char GetEscapeChar() const { return m_nEscapeChar; }
	const char *GetDelimiter() const { return m_pDelimiter; }
	int GetDelimiterLength() const { return m_nDelimiterLength; }

	const char *GetConversionString( char c ) const { return m_pReplacements[(unsigned char)c].m_pReplacementString; }
	int GetConversionLength( char c ) const { return m_pReplacements[(unsigned char)c].m_nLength; }
	int MaxConversionLength() const { return m_nMaxConversionLength; }

	// Finds a conversion for the passed-in string, returns length
	virtual char FindConversion( const char *pString, int *pLength );

protected:
	struct ConversionInfo_t
	{
		int m_nLength;
		const char *m_pReplacementString;
	};

	char m_nEscapeChar;
	const char *m_pDelimiter;
	int m_nDelimiterLength;
	int m_nCount;
	int m_nMaxConversionLength;
	char m_pList[255];
	ConversionInfo_t m_pReplacements[255];
};

#define BEGIN_CHAR_CONVERSION( _name, _delimiter, _escapeChar )	\
	static CUtlCharConversion::ConversionArray_t s_pConversionArray ## _name[] = {

#define END_CHAR_CONVERSION( _name, _delimiter, _escapeChar ) \
	}; \
	CUtlCharConversion _name( _escapeChar, _delimiter, sizeof( s_pConversionArray ## _name ) / sizeof( CUtlCharConversion::ConversionArray_t ), s_pConversionArray ## _name );

#define BEGIN_CUSTOM_CHAR_CONVERSION( _className, _name, _delimiter, _escapeChar ) \
	static CUtlCharConversion::ConversionArray_t s_pConversionArray ## _name[] = {

#define END_CUSTOM_CHAR_CONVERSION( _className, _name, _delimiter, _escapeChar ) \
	}; \
	_className _name( _escapeChar, _delimiter, sizeof( s_pConversionArray ## _name ) / sizeof( CUtlCharConversion::ConversionArray_t ), s_pConversionArray ## _name );

//-----------------------------------------------------------------------------
// Character conversions for C strings
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE CUtlCharConversion *GetCStringCharConversion();

//-----------------------------------------------------------------------------
// Character conversions for JSON strings
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE CUtlCharConversion *GetJSONCharConversion();

#endif