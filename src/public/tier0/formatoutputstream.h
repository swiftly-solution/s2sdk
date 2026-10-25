#ifndef FORMATOUTPUTSTREAM_H
#define FORMATOUTPUTSTREAM_H

#ifdef _WIN32
#pragma once
#endif

#include "platform_base.h"
#include "bufferstring.h"
#include <climits>
#include <initializer_list>
#include <string_view>
#include <utility>

class IFormatOutputStream;

class CFormatStringElement
{
public:
	CFormatStringElement() : m_unk001( 0 ) {}
	DLL_CLASS_IMPORT CFormatStringElement( const char *pszElement );

	DLL_CLASS_IMPORT const char *ParseElement( const char *pszFormat );
	DLL_CLASS_IMPORT void EnforceTypeAndSize( const char *pszTypes, const char *pszSize );

	operator const char *() const { return m_szElement; }

private:
	uint16_t m_unk001;
	char m_szElement[12];
};

class CTypeErasedFormattingWrapper
{
public:
	typedef void (*FormatFunc_t)( IFormatOutputStream *pStream, CFormatStringElement element, const void *pValue );

	const void *m_pValue;
	FormatFunc_t m_pfnFormat;
};

class PLATFORM_CLASS IFormatOutputStream
{
public:
	enum EOutputFlags_t
	{
        DEFAULT = 0,
		UNICODE_OK = (1 << 0),
		HTML = (1 << 1),
		MULTI_LINE = (1 << 2)
	};

	IFormatOutputStream() : m_nFlags( DEFAULT ), m_nCharsWritten( 0 ), m_unk101( 64 ), m_unk102( 1 ), m_unk103( INT_MAX ) {}
	virtual ~IFormatOutputStream() {}

	virtual void PutC( char c ) = 0;
	virtual void PutS( std::string_view str );
	virtual void PutRepeatedChar( char c, int nCount );

	virtual void FormatArray( CTypeErasedFormattingWrapper wrapper, CFormatStringElement element, const void **ppValues, int nCount, bool bTruncated );
	virtual void Format2DTable( CTypeErasedFormattingWrapper wrapper, CFormatStringElement element, const void **ppValues, int nColumns, int nRows );
	virtual void FormatEnumHelper( CFormatStringElement element, int nValue, std::initializer_list< std::pair< int, const char * > > names );
	virtual void FormatMaskHelper( int nValue, std::initializer_list< std::pair< int, const char * > > names );

	virtual void Terminate();
	virtual void BeginItem();
	virtual void EndItem();
	virtual int RemainingCapacity() const;
	virtual bool IsFull() const;
	virtual void OutputIndentation();
	virtual void SetTargetLine( int nLine );
	virtual void SetAnchor();

	bool UnicodeOK() const { return ( m_nFlags & UNICODE_OK ) != 0; }
	bool IsHTML() const { return ( m_nFlags & HTML ) != 0; }
	bool IsMultiLine() const { return ( m_nFlags & MULTI_LINE ) != 0; }
	bool IsNonFull() const { return !IsFull(); }

	void NewLineAndIndent() { PutC( '\n' ); OutputIndentation(); }

	int GetCharsWritten() const { return m_nCharsWritten; }

protected:
	EOutputFlags_t m_nFlags;
	int m_nCharsWritten;
	int m_unk101;
	int m_unk102;
	int m_unk103;
};

class PLATFORM_CLASS CBufferStringFormatOutputStream : public IFormatOutputStream
{
public:
	CBufferStringFormatOutputStream() : m_pString( nullptr ) {}
	CBufferStringFormatOutputStream( CBufferString &str ) : m_pString( &str ) {}

	void SetString( CBufferString &str ) { m_pString = &str; }

	virtual void PutC( char c );

private:
	CBufferString *m_pString;
};

class PLATFORM_CLASS CCharBufferFormatOutputStream : public IFormatOutputStream
{
public:
	CCharBufferFormatOutputStream( char *pBuffer, int nBufferSize ) : m_pCurrent( pBuffer ), m_pEnd( pBuffer + nBufferSize ) {}

	virtual void PutC( char c );
	virtual void PutS( std::string_view str );
	virtual void Terminate();
	virtual int RemainingCapacity() const;
	virtual bool IsFull() const;

private:
	char *m_pCurrent;
	char *m_pEnd;
};

#endif