//===== Copyright 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef TIER1_STRTOOLS_H
#define TIER1_STRTOOLS_H

#include <ctype.h>
#include <wchar.h>
#include <stdarg.h>
#include "platform_base.h"
#include "basetypes.h"
#include "annotations.h"
#include "unicode.h"
#include "stdlib.h"
class Vector2D;
class Vector;
class Vector4D;
class QAngle;
class Quaternion;
class VectorWS;
#include "color.h"
#include "errorlistener.h"

#ifdef _WIN32
#pragma once
#endif

#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "utlleanvector.h"
#include "utlvector.h"

// Forward declaration
class CBufferString;
class Vector;
class Vector2D;
class Vector4D;
class VectorWS;
class Quaternion;
class Color;
class QAngle;
class CUtlString;
class IErrorListener;

#define PARSING_FLAG_NONE							(0)
#define PARSING_FLAG_ERROR_ASSERT					(1 << 0) // Triggers debug assertion on parsing errors, the default state
#define PARSING_FLAG_SKIP_ASSERT					(1 << 1) // Internal flag that is set when assertion is triggered, could also be used to prevent debug assertions
#define PARSING_FLAG_EMIT_WARNING					(1 << 2) // Emits global console warning on parsing errors, the default state
#define PARSING_FLAG_SKIP_WARNING					(1 << 3) // Internal flag that is set when global warning is emitted, could also be used to prevent warning messages
#define PARSING_FLAG_SILENT							(1 << 4) // Won't call callback when parsing errors are encountered
#define PARSING_FLAG_ERROR_IF_EMPTY					(1 << 5) // Emits parsing error if the input string was empty or NULL
#define PARSING_FLAG_UNK006							(1 << 6)
#define PARSING_FLAG_USE_BASE_AUTO					(1 << 7) // Use auto detection of a number base when parsing (uses https://en.cppreference.com/w/cpp/string/basic_string/stol under the hood, so same base rules applies)
#define PARSING_FLAG_USE_BASE_16					(1 << 8) // Use base of 16 when parsing
#define PARSING_FLAG_USE_PARTIAL_RESULT				(1 << 9) // Integer parsing that stops before the end of the string returns what it parsed so far instead of the default

// UNDONE: Find a non-compiler-specific way to do this
#ifdef _WIN32
#ifndef _VA_LIST_DEFINED

#ifdef  _M_ALPHA

struct va_list 
{
    char *a0;       /* pointer to first homed integer argument */
    int offset;     /* byte offset of next parameter */
};

#else  // !_M_ALPHA

typedef char *  va_list;

#endif // !_M_ALPHA

#define _VA_LIST_DEFINED

#endif   // _VA_LIST_DEFINED

#elif POSIX
#include <stdarg.h>
#endif

//-----------------------------------------------------------------------------
// Portable versions of standard string functions
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE int			V_tier0_strlen( const char *str );
PLATFORM_INTERFACE int			V_tier0_strlen16( const uchar16 *str );
PLATFORM_INTERFACE int			V_tier0_strlen32( const uchar32 *str );
PLATFORM_INTERFACE int			V_tier0_wcslen( const wchar_t *str );

PLATFORM_INTERFACE void			V_tier0_strcpy( char *dest, const char *src );
PLATFORM_INTERFACE void			_V_strncpy( char *pDest, const char *pSrc, int maxLen );
PLATFORM_INTERFACE void			V_tier0_strcpy32( uchar32 *dest, const uchar32 *src );
PLATFORM_INTERFACE void			_V_strncpy32_bytes( uchar32 *pDest, const uchar32 *pSrc, int bytes );
PLATFORM_INTERFACE void			V_tier0_wcscpy( wchar_t *dest, const wchar_t *src );
PLATFORM_INTERFACE void			_V_wcsncpy_bytes( OUT_Z_BYTECAP( bytes ) wchar_t *pDest, const wchar_t *pSrc, int bytes );

PLATFORM_INTERFACE const char *	V_tier0_strchr( const char *s, char c );
PLATFORM_INTERFACE char *		V_tier0_strrchr( const char *s, char c );
PLATFORM_INTERFACE char *		V_strnchr( const char *s, char c, int n );
PLATFORM_INTERFACE uchar32 *	V_strchr32( const uchar32 *s, uchar32 c );
PLATFORM_INTERFACE wchar_t *	V_tier0_wcschr( const wchar_t *s, wchar_t c );

PLATFORM_INTERFACE int			V_tier0_strcmp( const char *s1, const char *s2 );
PLATFORM_INTERFACE int			_V_strncmp( const char *s1, const char *s2, int n );
PLATFORM_INTERFACE int			V_strcmp32( const uchar32 *s1, const uchar32 *s2 );
PLATFORM_INTERFACE int			V_tier0_wcscmp( const wchar_t *s1, const wchar_t *s2 );

PLATFORM_INTERFACE int			V_stricmp_fast( const char *s1, const char *s2 );
// A special high-performance case-insensitive compare function that in
// a single call distinguishes between exactly matching strings,
// strings equal in case-insensitive way, and not equal strings:
//   returns 0 if strings match exactly
//   returns >0 if strings match in a case-insensitive way, but do not match exactly
//   returns <0 if strings do not match even in a case-insensitive way
PLATFORM_INTERFACE int			V_stricmp_fast_NegativeForUnequal( const char *s1, const char *s2 );
PLATFORM_INTERFACE int			_V_strnicmp_fast( const char *s1, const char *s2, int n );
PLATFORM_INTERFACE int			V_wcsicmp( const wchar_t *s1, const wchar_t *s2 );
PLATFORM_INTERFACE int			V_wcsnicmp_cch( const wchar_t *s1, const wchar_t *s2, int symbols );

PLATFORM_INTERFACE char *		V_tier0_strstr( const char *s1, const char *search );
PLATFORM_INTERFACE uchar32 *	V_strstr32( const uchar32 *s1, const uchar32 *search );

PLATFORM_INTERFACE char *		V_strupper_fast( char *start );
PLATFORM_INTERFACE uchar32 *	V_towupper32( uchar32 *start );
PLATFORM_INTERFACE wchar_t *	V_towupper( wchar_t *start );

PLATFORM_INTERFACE char *		V_strlower_fast( char *start );
PLATFORM_INTERFACE uchar32 *	V_towlower32( uchar32 *start );
PLATFORM_INTERFACE wchar_t *	V_towlower( wchar_t *start );

PLATFORM_INTERFACE int64 		V_atoi64( const char *str );
PLATFORM_INTERFACE uint64 		V_atoui64( const char *str );
PLATFORM_INTERFACE double		V_atof( const char *str );
PLATFORM_INTERFACE float		V_atofloat32( const char *str );
PLATFORM_INTERFACE double		V_atofloat64( const char *str );

PLATFORM_INTERFACE double		V_strtod( const char *str, char **endptr = NULL );
PLATFORM_INTERFACE float		V_strtof( const char *str, char **endptr = NULL );
PLATFORM_INTERFACE double		V_wcstod( const wchar_t *str, wchar_t **endptr = NULL );
PLATFORM_INTERFACE int64		V_strtoi64( const char *str, char **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE int64		V_wcstoi64( const wchar_t *str, wchar_t **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE uint64		V_strtoui64( const char *str, char **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE uint64		V_wcstoui64( const wchar_t *str, wchar_t **endptr = NULL, int base = 10 );
// Clamp the value to the int32/uint32 range
PLATFORM_INTERFACE int32		V_strtoi32( const char *str, char **endptr = NULL, int base = 10 );
PLATFORM_INTERFACE uint32		V_strtoui32( const char *str, char **endptr = NULL, int base = 10 );

PLATFORM_OVERLOAD const char *	V_stristr_fast( const char *str, const char *search );
PLATFORM_INTERFACE const char *	_V_strnistr_fast( const char *str, const char *search, int n );
PLATFORM_OVERLOAD const wchar_t *V_wcsistr( const wchar_t *str, const wchar_t *search );

PLATFORM_OVERLOAD int			V_strnlen( const char *str, int n );
PLATFORM_OVERLOAD int			V_strnlen( const uchar16 *str, int n );
PLATFORM_OVERLOAD int			V_strnlen( const uchar32 *str, int n );

PLATFORM_INTERFACE size_t		V_strspn( const char *str, const char *strCharSet );
PLATFORM_INTERFACE int			_V_strcspn( const char *s1, const char *s2 );

#define COPY_ALL_CHARACTERS -1
PLATFORM_INTERFACE char *		_V_strncat( INOUT_Z_CAP( size ) char *s1, const char *s2, size_t size, int max_chars_to_copy = COPY_ALL_CHARACTERS );
void V_strcat( INOUT_Z_CAP( cchDest ) char *dest, const char *src, int cchDest );
template <size_t cchDest> char *V_strcat_safe( INOUT_Z_ARRAY char (&pDest)[cchDest], const char *pSrc, int nMaxCharsToCopy=COPY_ALL_CHARACTERS )
{ 
	return _V_strncat( pDest, pSrc, (int)cchDest, nMaxCharsToCopy );
}

PLATFORM_INTERFACE wchar_t *	_V_wcsncat_cch( INOUT_Z_CAP( cchDest ) wchar_t *pDest, const wchar_t *pSrc, int cchDest, int nMaxCharsToCopy=COPY_ALL_CHARACTERS );
void V_wcscat( INOUT_Z_CAP(cchDest) wchar_t *dest, const wchar_t *src, int cchDest );
template <size_t cchDest> wchar_t *V_wcscat_safe( INOUT_Z_ARRAY wchar_t( &pDest )[cchDest], const wchar_t *pSrc, int nMaxCharsToCopy=COPY_ALL_CHARACTERS )
{
	return _V_wcsncat_cch( pDest, pSrc, cchDest, nMaxCharsToCopy );
}

PLATFORM_INTERFACE int			V_snprintfcat( OUT_Z_CAP( destLen ) char *pDest, int destLen, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );
PLATFORM_INTERFACE int			V_snwprintf_bytes( OUT_Z_CAP( bytes ) wchar_t *pDest, int bytes, PRINTF_FORMAT_STRING const wchar_t *pFormat, ... );
PLATFORM_INTERFACE int			V_snwprintf_cch( OUT_Z_CAP( symbols ) wchar_t *pDest, int symbols, PRINTF_FORMAT_STRING const wchar_t *pFormat, ... );
PLATFORM_INTERFACE int			V_snprintfNoSecurityDoNotUse( OUT_Z_CAP( maxLenInChars ) char *pDest, int maxLenInChars, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 3, 4 );

PLATFORM_INTERFACE int			V_vsnprintf( OUT_Z_CAP( maxLenInCharacters ) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );
PLATFORM_INTERFACE int			V_vsnprintfcat( OUT_Z_CAP( maxLenInCharacters ) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );
template <size_t maxLenInCharacters> int V_vsprintf_safe( OUT_Z_ARRAY char (&pDest)[maxLenInCharacters], PRINTF_FORMAT_STRING const char *pFormat, va_list params ) { return V_vsnprintf( pDest, maxLenInCharacters, pFormat, params ); }

template <size_t maxLenInChars> int V_sprintf_safe( OUT_Z_ARRAY char( &pDest )[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 2, 3 );
template <size_t maxLenInChars> int V_sprintf_safe( OUT_Z_ARRAY char( &pDest )[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int result = V_vsnprintf( pDest, maxLenInChars, pFormat, params );
	va_end( params );
	return result;
}

// Append formatted text to an array in a safe manner -- always null-terminated, truncation rather than buffer overrun.
template <size_t maxLenInChars> int V_sprintfcat_safe( INOUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 2, 3 );
template <size_t maxLenInChars> int V_sprintfcat_safe( INOUT_Z_ARRAY char (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const char *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	size_t usedLength = V_strlen(pDest);
	// This code is here to check against buffer overruns when uninitialized arrays are passed in.
	// It should never be executed. Unfortunately we can't assert in this header file.
	if ( usedLength >= maxLenInChars )
		usedLength = 0;
	int result = V_vsnprintf( pDest + usedLength, maxLenInChars - usedLength, pFormat, params );
	va_end( params );
	return result;
}

PLATFORM_INTERFACE int			V_vsnwprintf_cch( OUT_Z_CAP( maxLenInCharacters ) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, va_list params );
template <size_t maxLenInCharacters> int V_vswprintf_safe( OUT_Z_ARRAY wchar_t( &pDest )[maxLenInCharacters], PRINTF_FORMAT_STRING const wchar_t *pFormat, va_list params ) { return V_vsnwprintf_cch( pDest, maxLenInCharacters, pFormat, params ); }

PLATFORM_INTERFACE int			V_vsnprintfRet( OUT_Z_CAP(maxLenInCharacters) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated );
PLATFORM_INTERFACE int			V_vsnwprintfRet_cch( OUT_Z_CAP( maxLenInChars ) wchar_t *pDest, int maxLenInChars, PRINTF_FORMAT_STRING const wchar_t *pFormat, va_list params, bool *pbTruncated );
template <size_t maxLenInCharacters> int V_vsprintfRet_safe( OUT_Z_ARRAY char (&pDest)[maxLenInCharacters], PRINTF_FORMAT_STRING const char *pFormat, va_list params, bool *pbTruncated ) { return V_vsnprintfRet( pDest, maxLenInCharacters, pFormat, params, pbTruncated ); }

// Warns when the string is truncated, unless bQuietTruncation is set
PLATFORM_INTERFACE int			V_snprintfWarnTrunc( OUT_Z_CAP( destLen ) char *pDest, int destLen, bool bQuietTruncation, PRINTF_FORMAT_STRING const char *pFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_INTERFACE int			V_vsnprintfWarnTrunc( OUT_Z_CAP( maxLenInCharacters ) char *pDest, int maxLenInCharacters, bool bQuietTruncation, PRINTF_FORMAT_STRING const char *pFormat, va_list params );

// Returns the length of the whole formatted string without the terminator. pDest can be null with a length of 0.
PLATFORM_INTERFACE int			V_vsnprintfSize( OUT_Z_CAP( maxLenInCharacters ) char *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const char *pFormat, va_list params );

// Returns the length of the formatted string without the terminator
PLATFORM_INTERFACE int			V_vscprintf( PRINTF_FORMAT_STRING const char *pFormat, va_list params );

// FMTFUNCTION can only be used on ASCII functions, not wide-char functions.
int V_snwprintf( OUT_Z_CAP(maxLenInCharacters) wchar_t *pDest, int maxLenInCharacters, PRINTF_FORMAT_STRING const wchar_t *pFormat, ... );
template <size_t maxLenInChars> int V_swprintf_safe( OUT_Z_ARRAY wchar_t (&pDest)[maxLenInChars], PRINTF_FORMAT_STRING const wchar_t *pFormat, ... )
{
	va_list params;
	va_start( params, pFormat );
	int result = V_vsnwprintf_cch( pDest, maxLenInChars, pFormat, params );
	va_end( params );
	return result;
}

// this is locale-unaware and therefore faster version of standard isdigit()
// It also avoids sign-extension errors.
bool V_isdigit( char c );

PLATFORM_INTERFACE bool V_isdigit_str( const char *str );
PLATFORM_INTERFACE int V_iswdigit( uchar32 c );

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

bool V_isempty( const char* pszString );

// The islower/isdigit/etc. functions all expect a parameter that is either
// 0-0xFF or EOF. It is easy to violate this constraint simply by passing
// 'char' to these functions instead of unsigned char.
// The V_ functions handle the char/unsigned char mismatch by taking a
// char parameter and casting it to unsigned char so that chars with the
// sign bit set will be zero extended instead of sign extended.
// Not that EOF cannot be passed to these functions.
//
// These functions could also be used for optimizations if locale
// considerations make some of the CRT functions slow.
bool V_isalpha(char c);
bool V_isalnum(char c);
bool V_isprint(char c);
bool V_isxdigit(char c);
bool V_ispunct(char c);
bool V_isgraph(char c);
bool V_isupper(char c);
bool V_islower(char c);
bool V_iscntrl(char c);
bool V_isspace(char c);
PLATFORM_INTERFACE int	V_iswspace( uchar32 c );
PLATFORM_INTERFACE int	V_iswalpha( uchar32 c );
PLATFORM_INTERFACE int	V_iswalnum( uchar32 c );
PLATFORM_INTERFACE int	V_iswcntrl( uchar32 c );
PLATFORM_INTERFACE int	V_iswlower( uchar32 c );
PLATFORM_INTERFACE int	V_iswupper( uchar32 c );
PLATFORM_INTERFACE int	V_iswpunct( uchar32 c );
PLATFORM_INTERFACE int	V_isbreakablewspace( wchar_t ch );
PLATFORM_INTERFACE int	V_isbreakablewspace32( uchar32 ch );
PLATFORM_INTERFACE int	V_tier0_ispunct( int c );
PLATFORM_INTERFACE int	V_tier0_isgraph( int c );

// Short form remaps
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

// Compares two strings with the support of wildcarding only for the first arg (includes '*' for multiple and '?' for single char usages)
PLATFORM_INTERFACE int V_CompareNameWithWildcards(const char *wildcarded_string, const char *compare_to, bool case_sensitive = false);

// Parses string equivalent of ("true", "false", "yes", "no", "1", "0") to the boolean value
// where default_value is what would be returned if parsing has failed
PLATFORM_INTERFACE bool V_StringToBool(const char *buf, bool default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a float array up to the amount of arr_size or up to the string limit, the amount of parsed values is returned
PLATFORM_INTERFACE int V_StringToFloatArray(const char *buf, float *out_arr, int arr_size, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into an int array up to the amount of arr_size or up to the string limit, the amount of parsed values is returned
PLATFORM_INTERFACE int V_StringToIntArray(const char *buf, int *out_arr, int arr_size, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a Vector structure
PLATFORM_INTERFACE void V_StringToVector(const char *buf, Vector &out_vec, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a Vector2D structure
PLATFORM_INTERFACE void V_StringToVector2D(const char *buf, Vector2D &out_vec, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a Vector4D structure
PLATFORM_INTERFACE void V_StringToVector4D(const char *buf, Vector4D &out_vec, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a Color structure
PLATFORM_INTERFACE void V_StringToColor(const char *buf, Color &out_clr, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a QAngle structure
PLATFORM_INTERFACE void V_StringToQAngle(const char *buf, QAngle &out_ang, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a Quaternion structure
PLATFORM_INTERFACE void V_StringToQuaternion(const char *buf, Quaternion &out_quat, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string into a VectorWS structure
PLATFORM_INTERFACE void V_StringToVectorWS(const char *buf, VectorWS &out_vecws, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a uint64 value, where if the value exceeds min/max limits (inclusive), the parsing fails and default_value is returned
PLATFORM_INTERFACE uint64 V_StringToUint64Limit(const char *buf, uint64 min, uint64 max, uint64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a int64 value, where if the value exceeds min/max limits (inclusive), the parsing fails and default_value is returned
PLATFORM_INTERFACE int64 V_StringToInt64Limit(const char *buf, int64 min, int64 max, int64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a uint64 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint64 V_StringToUint64(const char *buf, uint64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a int64 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int64 V_StringToInt64(const char *buf, int64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a uint32 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint32 V_StringToUint32(const char *buf, uint32 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a int32 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int32 V_StringToInt32(const char *buf, int32 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a uint16 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint16 V_StringToUint16(const char *buf, uint16 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a int16 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int16 V_StringToInt16(const char *buf, int16 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a uint8 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE uint8 V_StringToUint8(const char *buf, uint8 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a int8 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE int8 V_StringToInt8(const char *buf, int8 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a float64 value, where if the value exceeds min/max limits (inclusive), the parsing fails and default_value is returned
PLATFORM_INTERFACE float64 V_StringToFloat64Limit(const char *buf, float64 min, float64 max, float64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a float64 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE float64 V_StringToFloat64(const char *buf, float64 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a float32 value, if the parsing fails, default_value is returned
PLATFORM_INTERFACE float32 V_StringToFloat32(const char *buf, float32 default_value, bool *successful = NULL, char **remainder = NULL, uint flags = PARSING_FLAG_NONE, IErrorListener *err_listener = NULL);

// Parses string as a float64 value, if the parsing fails, default_value is returned, doesn't perform error checking/reporting
PLATFORM_INTERFACE float64 V_StringToFloat64Raw(const char *buf, float64 default_value, bool *successful = NULL, char **remainder = NULL);

// Parses string as a float32 value, if the parsing fails, default_value is returned, doesn't perform error checking/reporting
PLATFORM_INTERFACE float32 V_StringToFloat32Raw(const char *buf, float32 default_value, bool *successful = NULL, char **remainder = NULL);

// V_StringTo* variants that return whether parsing succeeded, storing the value (default_value if it failed) in out_value.
// Parsing errors only go to err_listener, the ERROR_ASSERT, EMIT_WARNING and SILENT flags are ignored.
PLATFORM_INTERFACE bool V_CheckStringToBool(const char *buf, IErrorListener *err_listener, uint32 flags, bool *out_value, bool default_value);
PLATFORM_INTERFACE bool V_CheckStringToInt8(const char *buf, IErrorListener *err_listener, uint32 flags, int8 *out_value, int8 default_value);
PLATFORM_INTERFACE bool V_CheckStringToUint8(const char *buf, IErrorListener *err_listener, uint32 flags, uint8 *out_value, uint8 default_value);
PLATFORM_INTERFACE bool V_CheckStringToInt16(const char *buf, IErrorListener *err_listener, uint32 flags, int16 *out_value, int16 default_value);
PLATFORM_INTERFACE bool V_CheckStringToUint16(const char *buf, IErrorListener *err_listener, uint32 flags, uint16 *out_value, uint16 default_value);
PLATFORM_INTERFACE bool V_CheckStringToInt32(const char *buf, IErrorListener *err_listener, uint32 flags, int32 *out_value, int32 default_value);
PLATFORM_INTERFACE bool V_CheckStringToUint32(const char *buf, IErrorListener *err_listener, uint32 flags, uint32 *out_value, uint32 default_value);
PLATFORM_INTERFACE bool V_CheckStringToInt64(const char *buf, IErrorListener *err_listener, uint32 flags, int64 *out_value, int64 default_value);
PLATFORM_INTERFACE bool V_CheckStringToUint64(const char *buf, IErrorListener *err_listener, uint32 flags, uint64 *out_value, uint64 default_value);
PLATFORM_INTERFACE bool V_CheckStringToInt64Limit(const char *buf, int64 min, int64 max, IErrorListener *err_listener, uint32 flags, int64 *out_value, int64 default_value);
PLATFORM_INTERFACE bool V_CheckStringToUint64Limit(const char *buf, uint64 min, uint64 max, IErrorListener *err_listener, uint32 flags, uint64 *out_value, uint64 default_value);
PLATFORM_INTERFACE bool V_CheckStringToFloat32(const char *buf, IErrorListener *err_listener, uint32 flags, float32 *out_value, float32 default_value);
PLATFORM_INTERFACE bool V_CheckStringToFloat64(const char *buf, IErrorListener *err_listener, uint32 flags, float64 *out_value, float64 default_value);
PLATFORM_INTERFACE bool V_CheckStringToFloat32Limit(const char *buf, float32 min, float32 max, IErrorListener *err_listener, uint32 flags, float32 *out_value, float32 default_value);
PLATFORM_INTERFACE bool V_CheckStringToFloat64Limit(const char *buf, float64 min, float64 max, IErrorListener *err_listener, uint32 flags, float64 *out_value, float64 default_value);
PLATFORM_INTERFACE bool V_CheckStringToVector(const char *buf, IErrorListener *err_listener, uint32 flags, Vector *out_value);
PLATFORM_INTERFACE bool V_CheckStringToVector2D(const char *buf, IErrorListener *err_listener, uint32 flags, Vector2D *out_value);
PLATFORM_INTERFACE bool V_CheckStringToVector4D(const char *buf, IErrorListener *err_listener, uint32 flags, Vector4D *out_value);
PLATFORM_INTERFACE bool V_CheckStringToVectorWS(const char *buf, IErrorListener *err_listener, uint32 flags, VectorWS *out_value);
PLATFORM_INTERFACE bool V_CheckStringToQAngle(const char *buf, IErrorListener *err_listener, uint32 flags, QAngle *out_value);
PLATFORM_INTERFACE bool V_CheckStringToQuaternion(const char *buf, IErrorListener *err_listener, uint32 flags, Quaternion *out_value);
PLATFORM_INTERFACE bool V_CheckStringToColor(const char *buf, IErrorListener *err_listener, uint32 flags, Color *out_value);

// Reports a parsing error the way the V_StringTo* functions do, according to flags
PLATFORM_INTERFACE void V_StringParseError(uint32 flags, IErrorListener *err_listener, PRINTF_FORMAT_STRING const char *fmt, ...) FMTFUNCTION( 3, 4 );

// Templatised and shortened version of the generic V_StringTo* functions
// these are silent, so you won't get error console warnings if parsing fails by default
template <typename T>
inline bool V_StringToValue( const char *string, T &value, uint flags = PARSING_FLAG_SKIP_ASSERT | PARSING_FLAG_SKIP_WARNING );

template <> inline bool V_StringToValue<bool>( const char *string, bool &value, uint flags )
{ bool success = false; value = V_StringToBool( string, false, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<int8>( const char *string, int8 &value, uint flags )
{ bool success = false; value = V_StringToInt8( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<uint8>( const char *string, uint8 &value, uint flags )
{ bool success = false; value = V_StringToUint8( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<int16>( const char *string, int16 &value, uint flags )
{ bool success = false; value = V_StringToInt16( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<uint16>( const char *string, uint16 &value, uint flags )
{ bool success = false; value = V_StringToUint16( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<int32>( const char *string, int32 &value, uint flags )
{ bool success = false; value = V_StringToInt32( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<uint32>( const char *string, uint32 &value, uint flags )
{ bool success = false; value = V_StringToUint32( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<int64>( const char *string, int64 &value, uint flags )
{ bool success = false; value = V_StringToInt64( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<uint64>( const char *string, uint64 &value, uint flags )
{ bool success = false; value = V_StringToUint64( string, 0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<float32>( const char *string, float32 &value, uint flags )
{ bool success = false; value = V_StringToFloat32( string, 0.0f, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<float64>( const char *string, float64 &value, uint flags )
{ bool success = false; value = V_StringToFloat64( string, 0.0, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<Vector>( const char *string, Vector &value, uint flags )
{ bool success = false; V_StringToVector( string, value, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<Vector2D>( const char *string, Vector2D &value, uint flags )
{ bool success = false; V_StringToVector2D( string, value, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<Vector4D>( const char *string, Vector4D &value, uint flags )
{ bool success = false; V_StringToVector4D( string, value, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<Color>( const char *string, Color &value, uint flags )
{ bool success = false; V_StringToColor( string, value, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<QAngle>( const char *string, QAngle &value, uint flags )
{ bool success = false; V_StringToQAngle( string, value, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<Quaternion>( const char *string, Quaternion &value, uint flags )
{ bool success = false; V_StringToQuaternion( string, value, &success, nullptr, flags ); return success; }
template <> inline bool V_StringToValue<VectorWS>( const char *string, VectorWS &value, uint flags )
{ bool success = false; V_StringToVectorWS( string, value, &success, nullptr, flags ); return success; }

// returns string immediately following prefix, (ie str+strlen(prefix)) or NULL if prefix not found
PLATFORM_INTERFACE const char *_V_StringAfterPrefix( const char *str, const char *prefix );
PLATFORM_INTERFACE const char *_V_StringAfterPrefixCaseSensitive( const char *str, const char *prefix );

// returns str past its leading whitespace
PLATFORM_INTERFACE char *_V_StringAfterLeadingWhitespace( char *str );

// returns true if pStr ends with pSuffix
PLATFORM_INTERFACE bool _V_strEndsWith( const char *pStr, const char *pSuffix, bool bCaseSensitive );

#define V_StringAfterPrefix(str, prefix)					_V_StringAfterPrefix((str), (prefix))
#define V_StringAfterPrefixCaseSensitive(str, prefix)		_V_StringAfterPrefixCaseSensitive((str), (prefix))

bool	V_StringHasPrefix             ( const char *str, const char *prefix );
bool	V_StringHasPrefixCaseSensitive( const char *str, const char *prefix );

// Normalizes a float string in place.  
// (removes leading zeros, trailing zeros after the decimal point, and the decimal point itself where possible)
PLATFORM_INTERFACE void	V_normalizeFloatString( char* pFloat );
PLATFORM_INTERFACE void	V_normalizeFloatWString( wchar_t* pFloat );

// Prints out a pretified memory counter string value ( e.g., 7,233.27 Mb, 1,298.003 Kb, 127 bytes )
PLATFORM_INTERFACE char *V_PrettifyMem( float value, int digitsafterdecimal = 2, bool usebinaryonek = false );

// Prints out a pretified integer with comma separators (eg, 7,233,270,000)
PLATFORM_INTERFACE char *V_PrettifyNum( int64 value );

// Prints out a pretified float with comma separators in the integer part (eg, 7,233.27)
PLATFORM_INTERFACE char *V_PrettifyFloat( float value, int digitsafterdecimal );

// Formats a duration as "1d.02:03:04", "2h:03:04", "3m:04" or "4s", followed by milliseconds (".005") and returns pOut's string.
// nFlags: 1 appends to pOut instead of replacing its contents, 2 leaves out the milliseconds.
PLATFORM_INTERFACE const char *V_PrettifyDurationMs( CBufferString *pOut, uint64 nDurationMs, uint32 nFlags );

// Write the shortest text representation of the value with a null terminator, pOut must be large enough
PLATFORM_INTERFACE void V_Int8ToString_Unsafe( int8 value, char *pOut );
PLATFORM_INTERFACE void V_UInt8ToString_Unsafe( uint8 value, char *pOut );
PLATFORM_INTERFACE void V_Int16ToString_Unsafe( int16 value, char *pOut );
PLATFORM_INTERFACE void V_UInt16ToString_Unsafe( uint16 value, char *pOut );
PLATFORM_INTERFACE void V_Int32ToString_Unsafe( int32 value, char *pOut );
PLATFORM_INTERFACE void V_UInt32ToString_Unsafe( uint32 value, char *pOut );
PLATFORM_INTERFACE void V_Int64ToString_Unsafe( int64 value, char *pOut );
PLATFORM_INTERFACE void V_UInt64ToString_Unsafe( uint64 value, char *pOut );
PLATFORM_INTERFACE void V_FloatToString_Unsafe( float value, char *pOut );
PLATFORM_INTERFACE void V_DoubleToString_Unsafe( double value, char *pOut );

// Like V_FloatToString_Unsafe and V_DoubleToString_Unsafe, but without exponent notation
PLATFORM_INTERFACE void V_FloatToString_NoExponent( float value, OUT_Z_CAP( nOutSize ) char *pOut, int nOutSize );
PLATFORM_INTERFACE void V_DoubleToString_NoExponent( double value, OUT_Z_CAP( nOutSize ) char *pOut, int nOutSize );

// Writes the number pIn without exponent notation (eg, 1.5e-3 as 0.0015), returns false if pOut is too small
PLATFORM_INTERFACE bool V_ExpandScientificNotation( OUT_Z_CAP( nOutSize ) char *pOut, const char *pIn, int nOutSize );

// Removes the trailing zeros after the decimal point, keeping at least one digit after it
PLATFORM_INTERFACE void V_StripExcessTrailingZeros( char *pStr );

// Functions for converting hexidecimal character strings back into binary data etc.
//
// e.g., 
// int output;
// V_hextobinary( "ffffffff", 8, &output, sizeof( output ) );
// would make output == 0xfffffff or -1
// Similarly,
// char buffer[ 9 ];
// V_binarytohex( &output, sizeof( output ), buffer, sizeof( buffer ) );
// would put "ffffffff" into buffer (note null terminator!!!)
PLATFORM_INTERFACE void _V_hextobinary( char const *in, int numchars, byte *out, int maxoutputbytes );
PLATFORM_INTERFACE void _V_binarytohex( const byte *in, int inputbytes, char *out, int outsize );

#define V_HexToBinary(in, numchars, out, maxoutputbytes)	_V_hextobinary((in), (numchars), (out), (maxoutputbytes))
#define V_BinaryToHex(in, inputbytes, out, outsize)			_V_binarytohex((in), (inputbytes), (out), (outsize))


// Tools for working with filenames
// Extracts the base name of a file (no path, no extension, assumes '/' or '\' as path separator)
PLATFORM_INTERFACE void _V_FileBase( const char *in, char *out, int maxlen );
#define V_FileBase _V_FileBase

// Remove the final characters of ppath if it's '\' or '/'.
PLATFORM_INTERFACE void V_StripTrailingSlash( char *ppath );

// Returns pPath without its first character if it's '\' or '/'.
PLATFORM_INTERFACE const char *V_StripLeadingSlash( const char *pPath );

// Remove the final characters of ppline if they are whitespace (uses V_isspace)
PLATFORM_INTERFACE void V_StripTrailingWhitespace( char *ppline );

// Remove the initial characters of ppline if they are whitespace (uses V_isspace)
PLATFORM_INTERFACE void V_StripLeadingWhitespace( char *ppline );

// Remove the initial and final characters of ppline if they are whitespace
PLATFORM_INTERFACE void V_StripSurroundingWhitespace( char *ppline );

// Remove the initial and final whitespace characters of pStr, returns its new length
PLATFORM_INTERFACE int V_StrTrim( char *pStr );

// Remove the initial/final characters of ppline if they are " quotes
PLATFORM_INTERFACE void V_StripSurroundingQuotes( char *ppline );

// Remove any extension from in and return resulting string in out
PLATFORM_INTERFACE void _V_StripExtension( const char *in, char *out, int outLen );
#define V_StripExtension _V_StripExtension

// Make path end with extension if it doesn't already have an extension
PLATFORM_INTERFACE void _V_DefaultExtension( char *path, const char *extension, int pathStringLength );
#define V_DefaultExtension _V_DefaultExtension

// Strips any current extension from path and ensures that extension is the new extension.
// NOTE: extension string MUST include the . character
PLATFORM_INTERFACE void _V_SetExtension( char *path, const char *extension, int pathStringLength );
#define V_SetExtension _V_SetExtension

// Removes any filename from path ( strips back to previous / or \ character )
PLATFORM_INTERFACE void V_StripFilename( char *path );

// Remove the final directory from the path
PLATFORM_INTERFACE bool _V_StripLastDir( char *dirName, int maxlen );
#define V_StripLastDir _V_StripLastDir

// Returns a pointer to the unqualified file name (no path) of a file name
PLATFORM_INTERFACE const char *V_UnqualifiedFileName( const char *in );

// Given a path and a filename, composes "path\filename", inserting the (OS correct) separator if necessary
PLATFORM_INTERFACE void _V_ComposeFileName( const char *path, const char *filename, char *dest, int destSize );
#define V_ComposeFileName _V_ComposeFileName

// Copy out the path except for the stuff after the final pathseparator
PLATFORM_INTERFACE bool _V_ExtractFilePath( const char *path, char *dest, int destSize );
#define V_ExtractFilePath _V_ExtractFilePath

// Copy out the file extension into dest
PLATFORM_INTERFACE void _V_ExtractFileExtension( const char *path, char *dest, int destSize );
#define V_ExtractFileExtension _V_ExtractFileExtension

PLATFORM_INTERFACE const char *V_GetFileExtension( const char *path );
// Returns empty string instead of null on failure
PLATFORM_INTERFACE const char *V_GetFileExtensionSafe( const char *path );

// This removes "./" and "../" from the pathname. pFilename should be a full pathname.
// Returns false if it tries to ".." past the root directory in the drive (in which case 
// it is an invalid path).
PLATFORM_INTERFACE bool V_RemoveDotSlashes( char *pFilename, char separator = CORRECT_PATH_SEPARATOR );

// If pPath is a relative path, this function makes it into an absolute path
// using the current working directory as the base, or pStartingDir if it's non-NULL.
// Returns false if it runs out of room in the string, or if pPath tries to ".." past the root directory.
PLATFORM_INTERFACE bool _V_MakeAbsolutePath( OUT_Z_CAP( outLen ) char *pOut, int outLen, const char *pPath, const char *pStartingDir = NULL, bool bLowercase = false );
#define V_MakeAbsolutePath _V_MakeAbsolutePath

// Makes the path after the "vpk:" prefixes of pInPathName absolute, keeping the prefixes in front.
// Returns false and copies pInPathName unchanged if it has no such prefix.
PLATFORM_INTERFACE bool V_MakeAbsoluteVPKPathName( OUT_Z_CAP( nBufLen ) char *pOut, int nBufLen, const char *pInPathName, const char *pStartingDir, bool bLowercaseName );

PLATFORM_INTERFACE void V_RemoveFormatSpecifications( const char *pszFrom, char *pszTo, size_t sizeDest );

// Creates a relative path given two full paths
// The first is the full path of the file to make a relative path for.
// The second is the full path of the directory to make the first file relative to
// Returns false if they can't be made relative (on separate drives, for example)
PLATFORM_INTERFACE bool _V_MakeRelativePath( const char *pFullPath, const char *pDirectory, OUT_Z_CAP( nBufLen ) char *pRelativePath, int nBufLen, bool );
#define V_MakeRelativePath _V_MakeRelativePath

// Fixes up a file name, removing dot slashes, fixing slashes, converting to lowercase, etc.
PLATFORM_INTERFACE void _V_FixupPathName( OUT_Z_CAP( nOutLen ) char *pOut, size_t nOutLen, const char *pPath, bool convert_to_lower = true );
#define V_FixupPathName _V_FixupPathName

// Adds a path separator to the end of the string if there isn't one already. Returns false if it would run out of space.
PLATFORM_INTERFACE void _V_AppendSlash( INOUT_Z_CAP( strSize ) char *pStr, int strSize, char separator = CORRECT_PATH_SEPARATOR );
#define V_AppendSlash _V_AppendSlash

// Returns true if the path is an absolute path.
PLATFORM_INTERFACE bool V_IsAbsolutePath( const char *pPath );

// Returns true if the path starts with "vpk:" or "ugc:", ignoring case.
PLATFORM_INTERFACE bool V_IsFakePath( const char *pStr );

// Returns the form of the path: 0 if invalid, 1 if relative, 2 if absolute, 3 if drive-relative (C:dir) and 4 if relative to the current drive's root (\dir).
// V_GetVPKPathForm also accepts the "vpk:" and "ugc:" prefixes.
PLATFORM_INTERFACE int V_GetPathForm( const char *pPath );
PLATFORM_INTERFACE int V_GetVPKPathForm( const char *pPath );

// Returns the length of the root at the start of the path (like C:\ or /)
PLATFORM_INTERFACE int V_GetPathOriginLength( const char *pPath );

// Returns true if both paths are equal, ignoring case and treating '\' and '/' alike.
// With bDirectoryNames, a trailing slash is ignored too.
PLATFORM_INTERFACE bool V_AreFileNamesEqual( const char *pFileName1, const char *pFileName2, bool bDirectoryNames );
PLATFORM_INTERFACE bool V_PathsMatch( const char *pPath1, const char *pPath2 );

// Returns true if both paths are equal after V_FixupPathName, ignoring case
PLATFORM_INTERFACE bool V_PathNamesAreEquivalent( const char *pFirst, const char *pSecond );

// Returns true if pInner is inside pOuter, or equal to it with bAllowMatch
PLATFORM_INTERFACE bool V_PathContainsPath( const char *pOuter, const char *pInner, bool bAllowMatch );

// Shortens pPath to nMaxCharsAllowed by replacing its start with pReplacementString, returns the start of the shortened path within pPath
PLATFORM_INTERFACE char *V_AbbreviatePathInPlace( char *pPath, int nMaxCharsAllowed, const char *pReplacementString );

// Scans pIn and replaces all occurences of pMatch with pReplaceWith.
// Writes the result to pOut.
// Returns true if it completed successfully.
// If it would overflow pOut, it fills as much as it can and returns false.
PLATFORM_INTERFACE bool _V_StrSubst( const char *pIn, const char *pMatch, const char *pReplaceWith,
									 OUT_Z_CAP( outLen ) char *pOut, int outLen, bool bCaseSensitive=false );
#define V_StrSubst _V_StrSubst

// This function takes a slice out of pStr and stores it in pOut.
// It follows the Python slice convention:
// Negative numbers wrap around the string (-1 references the last character).
// Large numbers are clamped to the end of the string.
PLATFORM_INTERFACE void _V_StrSlice( const char *pStr, int firstChar, int lastCharNonInclusive, char *pOut, int outSize );
#define V_StrSlice _V_StrSlice

// Chop off the left nChars of a string.
PLATFORM_INTERFACE void _V_StrLeft( const char *pStr, int nChars, char *pOut, int outSize );
#define V_StrLeft _V_StrLeft

// Chop off the right nChars of a string.
PLATFORM_INTERFACE void _V_StrRight( const char *pStr, int nChars, char *pOut, int outSize );
#define V_StrRight _V_StrRight

// change "special" characters to have their c-style backslash sequence. like \n, \r, \t, ", etc.
// returns a pointer to a newly allocated string, which you must delete[] when finished with.
PLATFORM_INTERFACE char *V_AddBackSlashesToSpecialChars( char const *pSrc );

// Replaces \n, \r and \t in pStr with the characters they stand for, and other backslash sequences with the character after the backslash
PLATFORM_INTERFACE void V_DecodeBackslashSequencesInString( char *pStr );

// Force slashes of either type to be = separator character
PLATFORM_INTERFACE void V_FixSlashes( char *pname, char separator = CORRECT_PATH_SEPARATOR );

// This function fixes cases of filenames like materials\\blah.vmt or somepath\otherpath\\ and removes the extra double slash.
PLATFORM_INTERFACE void V_FixDoubleSlashes( char *pStr );

// Returns true if all slashes in pname are the separator character
PLATFORM_INTERFACE bool V_AreSlashesCorrect( const char *pname, char separator );

// Replace the characters that aren't valid in a file name or path with '_'
PLATFORM_INTERFACE void V_FixInvalidFileNameCharacters( char *pFileName );
PLATFORM_INTERFACE void V_FixInvalidPathCharacters( char *pFileName );

// Replace every ch or space in pPath with '_'
PLATFORM_INTERFACE void V_FixupPathCharToUnderscore( char *pPath, char ch );
PLATFORM_INTERFACE void V_FixupPathSpaceToUnderscore( char *pPath );

// Convert \r\n (Windows linefeeds) to \n (Unix linefeeds).
PLATFORM_INTERFACE void V_TranslateLineFeedsToUnix( char *pStr );

// Returns the number of line breaks in the first nLength characters of pText, or all of it if nLength is negative.
// \r\n counts as one line break.
PLATFORM_INTERFACE int V_CountTextLines( const char *pText, int nLength );

// Encode a string for display as HTML -- this only encodes ' " & < >, which are the important ones to encode for 
// security and ensuring HTML display doesn't break.  Other special chars like the ? sign and so forth will not
// be encoded
//
// Returns false if there was not enough room in pDest to encode the entire source string, otherwise true
PLATFORM_INTERFACE bool V_BasicHtmlEntityEncode( OUT_Z_CAP( nDestSize ) char *pDest, const int nDestSize, char const *pIn, const int nInSize, bool bPreserveWhitespace = false );


// Decode a string with htmlentities HTML -- this should handle all special chars, not just the ones V_BasicHtmlEntityEncode uses.
//
// Returns false if there was not enough room in pDest to decode the entire source string, otherwise true
PLATFORM_INTERFACE bool V_HtmlEntityDecodeToUTF8( OUT_Z_CAP( nDestSize ) char *pDest, const int nDestSize, char const *pIn, const int nInSize );

// Encode and decode the XML entities of the first nInSize bytes of pIn, or all of it if nInSize is negative, and return pOut's string.
// nFlags: 1 appends to pOut instead of replacing its contents, 4 also encodes spaces as &nbsp; and line breaks as <br>.
// V_XMLDecode returns NULL if pIn isn't valid UTF-8 or pOut runs out of space.
PLATFORM_INTERFACE const char *V_XMLEncode( const char *pIn, CBufferString *pOut, int nInSize, uint32 nFlags );
PLATFORM_INTERFACE const char *V_XMLDecode( const char *pIn, CBufferString *pOut, int nInSize, uint32 nFlags );

// strips HTML from a string.  Should call Q_HTMLEntityDecodeToUTF8 afterward.
PLATFORM_INTERFACE void V_StripAndPreserveHTML( CBufferString *pbuffer, const char *pchHTML, const char **rgszPreserveTags, uint cPreserveTags, uint cMaxResultSize );
PLATFORM_INTERFACE void V_StripAndPreserveHTMLCore( CBufferString *pbuffer, const char *pchHTML, const char **rgszPreserveTags, uint cPreserveTags, const char **rgszNoCloseTags, uint cNoCloseTags, uint cMaxResultSize );

// Extracts the domain from a URL
PLATFORM_INTERFACE bool V_ExtractDomainFromURL( const char *pchURL, OUT_Z_CAP( cchDomain ) char *pchDomain, int cchDomain );

// returns true if the url passed in is on the specified domain
PLATFORM_INTERFACE bool V_URLContainsDomain( const char *pchURL, const char *pchDomain );

//-----------------------------------------------------------------------------
// returns true if the character is allowed in a URL, false otherwise
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool V_IsValidURLCharacter( const char *pch, int *pAdvanceBytes );

//-----------------------------------------------------------------------------
// returns true if the character is allowed in a DNS doman name, false otherwise
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE bool V_IsValidDomainNameCharacter( const char *pch, int *pAdvanceBytes );

 // Converts BBCode tags to HTML tags
PLATFORM_INTERFACE bool V_BBCodeToHTML( OUT_Z_CAP( nDestSize ) char *pDest, const int nDestSize, char const *pIn, const int nInSize );


// helper to identify "mean" spaces, which we don't like in visible identifiers
// such as player Name
PLATFORM_INTERFACE bool V_IsMeanSpaceW( wchar_t wch );

// helper to identify characters which are deprecated in Unicode,
// and we simply don't accept
PLATFORM_INTERFACE bool V_IsDeprecatedW( wchar_t wch );

//-----------------------------------------------------------------------------
// generic unique name helper functions
//-----------------------------------------------------------------------------

// returns -1 if no match, nDefault if pName==prefix, and N if pName==prefix+N
PLATFORM_INTERFACE int V_IndexAfterPrefix( const char *pName, const char *prefix, int nDefault = 0 );

// returns startindex if none found, 2 if "prefix" found, and n+1 if "prefixn" found
template < class NameArray >
int V_GenerateUniqueNameIndex( const char *prefix, const NameArray &nameArray, int startindex = 0 )
{
	if ( !prefix )
		return 0;

	int freeindex = startindex;

	int nNames = nameArray.Count();
	for ( int i = 0; i < nNames; ++i )
	{
		int index = V_IndexAfterPrefix( nameArray[ i ], prefix, 1 ); // returns -1 if no match, 0 for exact match, N for 
		if ( index >= freeindex )
		{
			// TODO - check that there isn't more junk after the index in pElementName
			freeindex = index + 1;
		}
	}

	return freeindex;
}

template < class NameArray >
bool V_GenerateUniqueName( char *name, int memsize, const char *prefix, const NameArray &nameArray )
{
	if ( name == NULL || memsize == 0 )
		return false;

	if ( prefix == NULL )
	{
		name[ 0 ] = '\0';
		return false;
	}

	int prefixLength = V_strlen( prefix );
	if ( prefixLength + 1 > memsize )
	{
		name[ 0 ] = '\0';
		return false;
	}

	int i = V_GenerateUniqueNameIndex( prefix, nameArray );
	if ( i <= 0 )
	{
		V_strncpy( name, prefix, memsize );
		return true;
	}

	int newlen = prefixLength + ( int )log10( ( float )i ) + 1;
	if ( newlen + 1 > memsize )
	{
		V_strncpy( name, prefix, memsize );
		return false;
	}

	V_snprintf( name, memsize, "%s%d", prefix, i );
	return true;
}

// trim right whitespace
PLATFORM_INTERFACE char *TrimRight( char *pString );

PLATFORM_INTERFACE const char *SkipBlanks( const char *pString );

// Reads a line of fp into pLine, with its line break if bKeepLineBreak is set. Returns false if nothing could be read.
PLATFORM_INTERFACE bool V_ReadLine( FILE *fp, CBufferString *pLine, bool bKeepLineBreak );

// Writes a dump of data to output with 24 bytes per line as text and hex, each line starting with label and " << " if input is set or " >> " otherwise.
// V_LogMultilineToArray adds each line to output instead.
PLATFORM_INTERFACE void V_LogMultiline( bool input, const char *label, const char *data, size_t len, CUtlString &output );
PLATFORM_INTERFACE void V_LogMultilineToArray( bool input, const char *label, const char *data, size_t len, CUtlVector<CUtlString> &output );

// Returns the number of Unicode code points to insert, delete or replace to turn one UTF-8 string into the other.
// pbInvalidUTF8 is set if either string isn't valid UTF-8.
PLATFORM_INTERFACE int V_LevenshteinDistance( const char *pString1, const char *pString2, bool *pbInvalidUTF8 );

// Returns the offset of pNeedle in pHayStack, or -1 if it's not found
PLATFORM_INTERFACE int V_BoyerMooreSearch( const byte *pNeedle, int nNeedleLength, const byte *pHayStack, int nHayStackLength );

// AMNOTE: Unless pString starts with pBuf, truncates pBuf to the start it shares with pString, ending at an underscore
PLATFORM_INTERFACE void V_CommonPrefixToUnderscore( CBufferString *pBuf, const char *pString );

// Copies the first nTextLength characters of pText, or all of it if nTextLength is negative, to pOut with " replaced by ' and returns pOut's string.
// pOut is cleared first unless bAppend is set.
PLATFORM_OVERLOAD const char *V_SanitizeTextForCommandLine( const char *pText, CBufferString *pOut, bool bAppend, int nTextLength );

// Returns nLen random letters and digits
PLATFORM_OVERLOAD CUtlString V_RandomString( int nLen );

#ifdef PLATFORM_WINDOWS
// Converts buf in place between the Windows code pages, returns its string or NULL if it couldn't be converted
PLATFORM_OVERLOAD const char *V_ConvertBetweenStringEncodings( CBufferString &buf, uint32 nFromCodePage, uint32 nToCodePage );
// Converts buf in place from Windows-1252 to UTF-8
PLATFORM_OVERLOAD const char *V_ConvertWin32ANSILatinToUTF8( CBufferString &buf );
#endif

// Encodes a string (or binary data) in URL encoding format, see rfc1738 section 2.2.
// Dest buffer should be 3 times the size of source buffer to guarantee it has room to encode.
PLATFORM_INTERFACE void V_URLEncodeRaw( OUT_Z_CAP(nDestLen) char *pchDest, int nDestLen, const char *pchSource, int nSourceLen );

// Decodes a string (or binary data) from URL encoding format, see rfc1738 section 2.2.
// Dest buffer should be at least as large as source buffer to gurantee room for decode.
// Dest buffer being the same as the source buffer (decode in-place) is explicitly allowed.
//
// Returns the amount of space actually used in the output buffer.  
PLATFORM_INTERFACE size_t V_URLDecodeRaw( OUT_CAP(nDecodeDestLen) char *pchDecodeDest, int nDecodeDestLen, const char *pchEncodedSource, int nEncodedSourceLen );

// Encodes a string (or binary data) in URL encoding format, this isn't the strict rfc1738 format, but instead uses + for spaces.  
// This is for historical reasons and HTML spec foolishness that lead to + becoming a de facto standard for spaces when encoding form data.
// Dest buffer should be 3 times the size of source buffer to guarantee it has room to encode.
PLATFORM_INTERFACE void V_URLEncode( OUT_Z_CAP(nDestLen) char *pchDest, int nDestLen, const char *pchSource, int nSourceLen );

// Decodes a string (or binary data) in URL encoding format, this isn't the strict rfc1738 format, but instead uses + for spaces.  
// This is for historical reasons and HTML spec foolishness that lead to + becoming a de facto standard for spaces when encoding form data.
// Dest buffer should be at least as large as source buffer to gurantee room for decode.
// Dest buffer being the same as the source buffer (decode in-place) is explicitly allowed.
//
// Returns the amount of space actually used in the output buffer.  
PLATFORM_INTERFACE size_t V_URLDecode( OUT_CAP(nDecodeDestLen) char *pchDecodeDest, int nDecodeDestLen, const char *pchEncodedSource, int nEncodedSourceLen );

// NOTE: This is for backward compatability!
// We need to DLL-export the Q methods in vstdlib but not link to them in other projects
#if !defined( VSTDLIB_BACKWARD_COMPAT )

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
#define	Q_atoi					V_atoi
#define	Q_atoi64				V_atoi64
#define Q_atoui64				V_atoui64
#define	Q_atof					V_atof
#define	Q_stristr				V_stristr
#define	Q_strnistr				V_strnistr
#define	Q_strnchr				V_strnchr
#define Q_normalizeFloatString	V_normalizeFloatString
#define Q_strncpy				V_strncpy
#define Q_wcsncpy				V_wcsncpy
#define Q_snprintf				V_snprintf
#define Q_snwprintf				V_snwprintf_bytes
#define Q_wcsncpy				V_wcsncpy
#define Q_strncat				V_strncat
#define Q_vsnprintf				V_vsnprintf
#define Q_pretifymem			V_PrettifyMem
#define Q_pretifynum			V_PrettifyNum
#define Q_hextobinary			V_HexToBinary
#define Q_binarytohex			V_BinaryToHex
#define Q_FileBase				V_FileBase
#define Q_StripTrailingSlash	V_StripTrailingSlash
#define Q_StripExtension		V_StripExtension
#define	Q_DefaultExtension		V_DefaultExtension
#define Q_SetExtension			V_SetExtension
#define Q_StripFilename			V_StripFilename
#define Q_StripLastDir			V_StripLastDir
#define Q_UnqualifiedFileName	V_UnqualifiedFileName
#define Q_ComposeFileName		V_ComposeFileName
#define Q_ExtractFilePath		V_ExtractFilePath
#define Q_ExtractFileExtension	V_ExtractFileExtension
#define Q_GetFileExtension		V_GetFileExtension
#define Q_RemoveDotSlashes		V_RemoveDotSlashes
#define Q_MakeAbsolutePath		V_MakeAbsolutePath
#define Q_AppendSlash			V_AppendSlash
#define Q_IsAbsolutePath		V_IsAbsolutePath
#define Q_StrSubst				V_StrSubst
#define Q_SplitString			V_SplitString
#define Q_StrSlice				V_StrSlice
#define Q_StrLeft				V_StrLeft
#define Q_StrRight				V_StrRight
#define Q_FixSlashes			V_FixSlashes
#define Q_strcat				V_strcat
#define Q_MakeRelativePath		V_MakeRelativePath
#define Q_FixupPathName			V_FixupPathName

#endif // !defined( VSTDLIB_DLL_EXPORT )

// Split the specified string on the specified separator.
// Returns a list of strings separated by pSeparator.
// You are responsible for freeing the contents of outStrings (call outStrings.PurgeAndDeleteElements).
PLATFORM_OVERLOAD void V_SplitString( const char *pString, const char *pSeparator, CUtlVector<CUtlString> &outStrings, bool include_empty = false );

// Just like V_SplitString, but it can use multiple possible separators.
PLATFORM_OVERLOAD void V_SplitStringInPlace( char *pString, const char *pSeparator, CUtlLeanVector<const char *> &outStrings );

// Parses the numbers pString lists separated by separator into outNumbers, returns the number of values parsed
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char separator, CUtlVector<int32> &outNumbers );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char separator, CUtlVector<uint32> &outNumbers );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char separator, CUtlVector<int64> &outNumbers );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char separator, CUtlVector<uint64> &outNumbers );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char separator, CUtlVector<float> &outNumbers );
PLATFORM_OVERLOAD int V_SplitNumbers( const char *pString, char separator, CUtlVector<double> &outNumbers );

// Writes the numbers separated by separator to pOut and returns its string
PLATFORM_OVERLOAD const char *V_JoinNumbers( char separator, const CUtlVector<int32> &numbers, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char separator, const CUtlVector<uint32> &numbers, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char separator, const CUtlVector<int64> &numbers, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char separator, const CUtlVector<uint64> &numbers, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char separator, const CUtlVector<float> &numbers, CUtlString *pOut );
PLATFORM_OVERLOAD const char *V_JoinNumbers( char separator, const CUtlVector<double> &numbers, CUtlString *pOut );

struct characterset_t
{
	char set[256];
};


// This is essentially a strpbrk() using a precalculated lookup table
//-----------------------------------------------------------------------------
// Purpose: builds a simple lookup table of a group of important characters
// Input  : *pSetBuffer - pointer to the buffer for the group
//			*pSetString - list of characters to flag
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE void CharacterSetBuild( characterset_t *pSetBuffer, const char *pSetString );


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSetBuffer - pre-build group buffer
//			character - character to lookup
// Output : int - 1 if the character was in the set
//-----------------------------------------------------------------------------
#define IN_CHARACTERSET( SetBuffer, character )		((SetBuffer).set[static_cast<size_t>(character)])

// Returns true if buffer has a token before its next line break
PLATFORM_INTERFACE bool V_TokenWaiting( const char *buffer );

// Copies the line at pStrIn to pLine without its line break and returns the position after the line break, or NULL if there's none
PLATFORM_INTERFACE const char *V_ParseLine( const char *pStrIn, CBufferString *pLine );

// Copies the next token of pStrIn to pToken and returns the position after it, or NULL if there's none.
// Skips whitespace and // comments, handles quoted strings and makes every character of pTokenBreakCharacters
// a token of its own ({}()': if NULL). pbOverflowed is set if the token didn't fit in bufsize.
PLATFORM_INTERFACE const char *V_ParseToken( const char *pStrIn, char *pToken, int bufsize, bool *pbOverflowed, const characterset_t *pTokenBreakCharacters );

//-----------------------------------------------------------------------------
// Description of character conversions for string output
// Here's an example of how to use the macros to define a character conversion
// BEGIN_CHAR_CONVERSION( CStringConversion, '\\' )
//	{ '\n', "n" },
//	{ '\t', "t" }
// END_CHAR_CONVERSION( CStringConversion, '\\' )
//-----------------------------------------------------------------------------
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

	char GetEscapeChar() const;
	const char *GetDelimiter() const;
	int GetDelimiterLength() const;

	const char *GetConversionString( char c ) const;
	int GetConversionLength( char c ) const;
	int MaxConversionLength() const;

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

#endif	// TIER1_STRTOOLS_H
