#ifndef TIER0_UNICODE_H
#define TIER0_UNICODE_H

#ifdef _WIN32
#pragma once
#endif

#include "annotations.h"
#include "basetypes.h"

// Unicode string conversion policies - what to do if an illegal sequence is encountered
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

// Returns the UTF8 encoded length in this byte
PLATFORM_INTERFACE int V_UTF8LenFromFirst( char c );

// Returns the number of bytes the first nStringUnits characters of pString, or all of it if nStringUnits is negative, take
// when encoded as UTF-8, without the null terminator
PLATFORM_OVERLOAD int V_GetUTF8BytesNeeded( const char *pString, int nStringUnits );
PLATFORM_OVERLOAD int V_GetUTF8BytesNeeded( const uchar16 *pString, int nStringUnits );
PLATFORM_OVERLOAD int V_GetUTF8BytesNeeded( const uchar32 *pString, int nStringUnits );

// Conversion functions, returning the number of bytes consumed
// Decode a single UTF-8 character to a uchar32, returns number of UTF-8 bytes parsed
PLATFORM_INTERFACE int V_UTF8ToUChar32( const char *str, uchar32 &result, bool &failed );
PLATFORM_INTERFACE int V_UTF32ToUChar32( const uchar32 *str, uchar32 &result, bool &failed );

// Decode a single UTF-16 character to a uchar32, returns number of UTF-16 characters (NOT BYTES) consumed
PLATFORM_INTERFACE int V_UTF16ToUChar32( const uchar16 *str, uchar32 &result, bool &failed );

PLATFORM_INTERFACE int V_UChar32ToUTF16( const uchar32 *str, uchar16 *result );
PLATFORM_INTERFACE int V_UChar32ToUTF8( const uchar32 *str, char *result );

// Conversion between Unicode string types (UTF-8, UTF-16, UTF-32). Deals with bytes, not element counts,
// to minimize harm from the programmer mistakes which continue to plague our wide-character string code.
// Returns the number of bytes written to the output, or if output is NULL, the number of bytes required.
PLATFORM_INTERFACE int V_UTF8ToUTF16( const char *str, OUT_Z_BYTECAP( dest_size ) uchar16 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF8ToUTF32( const char *str, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16ToUTF8( const uchar16 *str, OUT_Z_BYTECAP( dest_size ) char *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16ToUTF16( const uchar16 *str, OUT_Z_BYTECAP( dest_size ) uchar16 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16ToUTF32( const uchar16 *str, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF32ToUTF8( const uchar32 *str, OUT_Z_BYTECAP( dest_size ) char *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF32ToUTF16( const uchar32 *str, OUT_Z_BYTECAP( dest_size ) uchar16 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );

// This is disgusting and exist only easily to facilitate having 16-bit and 32-bit wchar_t's on different platforms
PLATFORM_INTERFACE int V_UTF32ToUTF32( const uchar32 *str, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );

// Conversion between count-limited UTF-n character arrays, including any potential NULL characters.
// Output has a terminating NULL for safety; strip the last character if you want an unterminated string.
// Returns the number of bytes written to the output, or if output is NULL, the number of bytes required.
PLATFORM_INTERFACE int V_UTF8CharsToUTF16( const char *str, int size, OUT_Z_BYTECAP( dest_size ) uchar16 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF8CharsToUTF32( const char *str, int size, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16CharsToUTF8( const uchar16 *str, int size, OUT_Z_BYTECAP( dest_size ) char *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF16CharsToUTF32( const uchar16 *str, int size, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF32CharsToUTF8( const uchar32 *str, int size, OUT_Z_BYTECAP( dest_size ) char *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );
PLATFORM_INTERFACE int V_UTF32CharsToUTF16( const uchar32 *str, int size, OUT_Z_BYTECAP( dest_size ) uchar16 *dest, int dest_size, EStringConvertErrorPolicy policy = STRINGCONVERT_ASSERT_REPLACE );

// NOTE: WString means either UTF32 or UTF16 depending on the platform and compiler settings.
#if defined( _MSC_VER ) || defined( _WIN32 )
#define V_UTF8ToWString V_UTF8ToUTF16
#define V_UTF8CharsToWString V_UTF8CharsToUTF16
#define V_UTF32ToWString V_UTF32ToUTF16
#define V_WStringToUTF8 V_UTF16ToUTF8
#define V_WStringCharsToUTF8 V_UTF16CharsToUTF8
#define V_WStringToUTF32 V_UTF16ToUTF32
#else
#define V_UTF8ToWString V_UTF8ToUTF32
#define V_UTF8CharsToWString V_UTF8CharsToUTF32
#define V_UTF32ToWString V_UTF32ToUTF32
#define V_WStringToUTF8 V_UTF32ToUTF8
#define V_WStringCharsToUTF8 V_UTF32CharsToUTF8
#define V_WStringToUTF32 V_UTF32ToUTF32
#endif

PLATFORM_OVERLOAD int V_UnicodeCaseCollate( const char *s1, const char *s2, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );

// Validate that a Unicode string is well-formed and contains only valid code points
PLATFORM_OVERLOAD bool V_UnicodeValidate( const char *pUTF8 );
PLATFORM_OVERLOAD bool V_UnicodeValidate( const uchar16 *pUTF16 );
PLATFORM_OVERLOAD bool V_UnicodeValidate( const uchar32 *pUTF32 );

// Returns length of string in Unicode code points (printed glyphs or non-printing characters)
PLATFORM_OVERLOAD int V_UnicodeLength( const char *pUTF8 );
PLATFORM_OVERLOAD int V_UnicodeLength( const uchar16 *pUTF16 );
PLATFORM_OVERLOAD int V_UnicodeLength( const uchar32 *pUTF32 );

PLATFORM_OVERLOAD int V_UnicodeBufferLength( char const *pUTF8, int size, bool &success );
PLATFORM_OVERLOAD int V_UnicodeBufferLength( const uchar16 *pUTF32, int size, bool &success );
PLATFORM_OVERLOAD int V_UnicodeBufferLength( const uchar32 *pUTF16, int size, bool &success );

// Repair invalid Unicode strings by dropping truncated characters and fixing improperly-double-encoded UTF-16 sequences.
// Unlike conversion functions which replace with '?' by default, a repair operation assumes that you know that something
// is wrong with the string (eg, mid-sequence truncation) and you just want to do the best possible job of cleaning it up.
// You can pass a REPLACE or FAIL policy if you would prefer to replace characters with '?' or clear the entire string.
// Returns nonzero on success, or 0 if the policy is FAIL and an invalid sequence was found.
PLATFORM_OVERLOAD int V_UnicodeRepair( char *pUTF8, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeRepair( uchar16 *pUTF16, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeRepair( uchar32 *pUTF32, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );

// Advance pointer forward by N Unicode code points (printed glyphs or non-printing characters), stopping at terminating null if encountered.
PLATFORM_OVERLOAD char *V_UnicodeAdvance( char *pUTF8, int nCharacters );
PLATFORM_OVERLOAD uchar16 *V_UnicodeAdvance( uchar16 *pUTF16, int nCharactersnCharacters );
PLATFORM_OVERLOAD uchar32 *V_UnicodeAdvance( uchar32 *pUTF32, int nChars );
const char *V_UnicodeAdvance( const char *pUTF8, int nCharacters );
const uchar16 *V_UnicodeAdvance( const uchar16 *pUTF16, int nCharacters );
const uchar32 *V_UnicodeAdvance( const uchar32 *pUTF32, int nCharacters );

// Truncate to maximum of N Unicode code points (printed glyphs or non-printing characters)
void V_UnicodeTruncate( char *pUTF8, int nCharacters );
void V_UnicodeTruncate( uchar16 *pUTF16, int nCharacters );
void V_UnicodeTruncate( uchar32 *pUTF32, int nCharacters );

PLATFORM_OVERLOAD int V_UnicodeCaseCompare( const char *s1, const char *s2, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeCaseCompare( const uchar16 *s1, const uchar16 *s2, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeCaseCompare( const uchar32 *s1, const uchar32 *s2, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );

PLATFORM_OVERLOAD int V_UnicodeCaseConvert( const char *pUTF8, OUT_Z_BYTECAP( dest_size ) char *dest, int dest_size, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeCaseConvert( const uchar16 *pUTF16, OUT_Z_BYTECAP( dest_size ) uchar16 *dest, int dest_size, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD int V_UnicodeCaseConvert( const uchar32 *pUTF32, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );

PLATFORM_OVERLOAD bool V_UnicodeCaseStringInString( const char *pUTF8, const char *substr, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD bool V_UnicodeCaseStringInString( const uchar16 *pUTF16, const uchar16 *substr, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD bool V_UnicodeCaseStringInString( const uchar32 *pUTF32, const uchar32 *substr, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );

PLATFORM_OVERLOAD uchar32 *V_UnicodeConvertAndCaseStr( const char *pUTF8, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, uchar32 **new_buf, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD uchar32 *V_UnicodeConvertAndCaseStr( const uchar16 *pUTF16, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, uchar32 **new_buf, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );
PLATFORM_OVERLOAD uchar32 *V_UnicodeConvertAndCaseStr( const uchar32 *pUTF32, OUT_Z_BYTECAP( dest_size ) uchar32 *dest, int dest_size, uchar32 **new_buf, int flags, EStringConvertErrorPolicy ePolicy = STRINGCONVERT_SKIP );

//
// This utility class is for performing UTF-8 <-> UTF-16 conversion.
// It is intended for use with function/method parameters.
//
// For example, you can call
//     FunctionTakingUTF16( CStrAutoEncode( utf8_string ).ToUChar16() )
// or
//     FunctionTakingUTF8( CStrAutoEncode( utf16_string ).ToString() )
//
// The converted string is allocated off the heap, and destroyed when
// the object goes out of scope.
//
// if the string cannot be converted, NULL is returned.
//
// This class doesn't have any conversion operators; the intention is
// to encourage the developer to get used to having to think about which
// encoding is desired.
//
class CStrAutoEncode
{
public:
	explicit CStrAutoEncode( const char *buf );

	explicit CStrAutoEncode( const uchar16 *buf );

	explicit CStrAutoEncode( const uchar32 *buf );

	CStrAutoEncode( const CStrAutoEncode &other );

	// returns the UTF-8 string, converting on the fly.
	const char *ToUTF8();
	const char* ToString();

	// returns the UTF-16 string, converting on the fly.
	const uchar16 *ToUTF16();
	const uchar16 *ToUChar16();

	// returns the UTF-32 string, converting on the fly.
	const uchar32 *ToUTF32();
	const uchar32 *ToUChar32();

	~CStrAutoEncode();

	PLATFORM_CLASS void Clear();
	PLATFORM_CLASS CStrAutoEncode &Copy( const CStrAutoEncode &other );

	// Creates a copy and stores it
	PLATFORM_CLASS void SetCopy( const char *buf, int nCount = -1 );
	PLATFORM_CLASS void SetCopy( const uchar16 *buf, int nCount = -1 );
	PLATFORM_CLASS void SetCopy( const uchar32 *buf, int nCount = -1 );

	// Creates a copy and coverts to UTF8 then stores it
	PLATFORM_CLASS void SetUTF8Copy( const uchar32 *buf );
	PLATFORM_CLASS void SetUTF8Copy( const uchar16 *buf );

	// Creates a copy and coverts to UTF16 then stores it
	PLATFORM_CLASS void SetUTF16Copy( const char *buf );
	PLATFORM_CLASS void SetUTF16Copy( const uchar32 *buf );

	// Creates a copy and coverts to UTF32 then stores it
	PLATFORM_CLASS void SetUTF32Copy( const char *buf );
	PLATFORM_CLASS void SetUTF32Copy( const uchar16 *buf );

private:
	PLATFORM_CLASS void InitEmpty();

	PLATFORM_CLASS void PopulateUTF8() const;
	PLATFORM_CLASS void PopulateUTF16() const;
	PLATFORM_CLASS void PopulateUTF32() const;

	// one of these pointers is an owned pointer; whichever
	// one is the encoding OTHER than the one we were initialized
	// with is the pointer we've allocated and must free.
	const char *m_pch;
	const uchar16 *m_char16;
	const uchar32 *m_char32;

	bool m_bHasUTF8 : 1;
	bool m_bOwnUTF8 : 1;

	bool m_bHasUTF16 : 1;
	bool m_bOwnUTF16 : 1;

	bool m_bHasUTF32 : 1;
	bool m_bOwnUTF32 : 1;

	bool m_bOriginal : 1;
};

#endif // TIER0_UNICODE_H
