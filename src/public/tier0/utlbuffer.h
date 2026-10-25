#ifndef UTLBUFFER_H
#define UTLBUFFER_H

#ifdef _WIN32
#pragma once
#endif

#include "cbyteswap.h"
#include "platform.h"
#include "utlleanvector.h"
#include "bufferstring.h"
#include "strtools.h"

#define SetUtlBufferOverflowFuncs( _get, _put )	\
	SetOverflowFuncs( static_cast <UtlBufferOverflowFunc_t>( _get ), static_cast <UtlBufferOverflowFunc_t>( _put ) )

template < class A >
static const char *GetFmtStr( int nRadix = 10, bool bPrint = true ) { Assert( 0 ); return ""; }

template <> inline const char *GetFmtStr< short >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 ); return "%hd"; }
template <> inline const char *GetFmtStr< uint16_t >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 ); return "%hu"; }
template <> inline const char *GetFmtStr< int >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 ); return "%d"; }
template <> inline const char *GetFmtStr< uint32_t >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 || nRadix == 16 ); return nRadix == 16 ? "%x" : "%u"; }
template <> inline const char *GetFmtStr< int64_t >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 ); return "%lld"; }
template <> inline const char *GetFmtStr< float >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 ); return "%f"; }
template <> inline const char *GetFmtStr< double >	( int nRadix, bool bPrint ) { Assert( nRadix == 10 ); return bPrint ? "%.15lf" : "%lf"; }

class CUtlBuffer
{
// Brian has on his todo list to revisit this as there are issues in some cases with CUtlVector using operator = instead of copy construtor in InsertMultiple, etc.
// The unsafe case is something like this:
//  CUtlVector< CUtlBuffer > vecFoo;
// 
//  CUtlBuffer buf;
//  buf.Put( xxx );
//  vecFoo.Insert( buf );
//
//  This will cause memory corruption when vecFoo is cleared
//
//private:
//	// Disallow copying
//	CUtlBuffer( const CUtlBuffer & );// { Assert( 0 ); }
//	CUtlBuffer &operator=( const CUtlBuffer & );//  { Assert( 0 ); return *this; }

public:
	enum SeekType_t
	{
		SEEK_HEAD = 0,
		SEEK_CURRENT,
		SEEK_TAIL
	};

	// flags
	enum BufferFlags_t
	{
		BF_NONE = 0,
		TEXT_BUFFER = 0x1,			// Describes how get + put work (as strings, or binary)
		EXTERNAL_GROWABLE = 0x2,	// This is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow happens when Putting.
		CONTAINS_CRLF = 0x4,		// For text buffers only, does this contain \n or \n\r?
		READ_ONLY = 0x8,			// For external buffers; prevents null termination from happening.
		AUTO_TABS_DISABLED = 0x10,	// Used to disable/enable push/pop tabs
	};

	// Overflow functions when a get or put overflows
	typedef bool (CUtlBuffer::*UtlBufferOverflowFunc_t)( int nSize );

	// Constructors for growable + external buffers for serialization/unserialization
	DLL_CLASS_IMPORT CUtlBuffer( int growSize = 0, int initSize = 0, CUtlBuffer::BufferFlags_t nFlags = BufferFlags_t::BF_NONE );
	DLL_CLASS_IMPORT CUtlBuffer( const void* pBuffer, int size, CUtlBuffer::BufferFlags_t nFlags = BufferFlags_t::BF_NONE );
	// This one isn't actually defined so that we catch contructors that are trying to pass a bool in as the third param.
	CUtlBuffer( const void *pBuffer, int size, bool crap );

	unsigned char	GetFlags() const;

	// NOTE: This will assert if you attempt to recast it in a way that
	// is not compatible. The only valid conversion is binary-> text w/CRLF
	DLL_CLASS_IMPORT void		SetBufferType( bool bIsText, bool bContainsCRLF );

	// Makes sure we've got at least this much memory
	DLL_CLASS_IMPORT void		EnsureCapacity( int num );

	// Access for direct read into buffer
	void *			AccessForDirectRead( int nBytes );
	DLL_CLASS_IMPORT const void *AccessGet( int nBytes );
	DLL_CLASS_IMPORT void *AccessPut( int nBytes );

	// Attaches the buffer to external memory....
	DLL_CLASS_IMPORT void		SetExternalBuffer( void* pMemory, int nSize, int nInitialPut, CUtlBuffer::BufferFlags_t nFlags = BufferFlags_t::BF_NONE );
	bool			IsExternallyAllocated() const;
	DLL_CLASS_IMPORT void		AssumeMemory( void *pMemory, int nSize, int nInitialPut, CUtlBuffer::BufferFlags_t nFlags = BufferFlags_t::BF_NONE );
	void			*Detach();
	DLL_CLASS_IMPORT void *DetachMemory();

	void ActivateByteSwappingIfBigEndian( void )
	{
	}


	// Controls endian-ness of binary utlbufs - default matches the current platform
	DLL_CLASS_IMPORT void	ActivateByteSwapping( bool bActivate );
	DLL_CLASS_IMPORT void	SetBigEndian( bool bigEndian );
	DLL_CLASS_IMPORT bool	IsBigEndian( void );

	DLL_CLASS_IMPORT void	AlignBuffer( int alignment );
	// Skips zero bytes until the get position is aligned, returning false on a non-zero byte or the buffer's end
	DLL_CLASS_IMPORT bool	AlignBufferGet( int alignment );

	// Resets the buffer; but doesn't free memory
	void			Clear();

	// Clears out the buffer; frees memory
	void			Purge();

	// Dump the buffer to stdout
	void			Spew( );

	DLL_CLASS_IMPORT void Swap( CUtlBuffer &other );
	DLL_CLASS_IMPORT void Swap( CUtlLeanVector<unsigned char> &other );

	DLL_CLASS_IMPORT bool WriteToFile( const char *, bool );
	DLL_CLASS_IMPORT bool WriteToFileIfDifferent( const char * );

	// Read stuff out.
	// Binary mode: it'll just read the bits directly in, and characters will be
	//		read for strings until a null character is reached.
	// Text mode: it'll parse the file, turning text #s into real numbers.
	//		GetString will read a string until a space is reached
	char			GetChar( );
	unsigned char	GetUnsignedChar( );
	short			GetShort( );
	unsigned short	GetUnsignedShort( );
	int				GetInt( );
	int64_t			GetInt64( );
	unsigned int	GetIntHex( );
	unsigned int	GetUnsignedInt( );
	float			GetFloat( );
	double			GetDouble( );
	void *			GetPtr();
	DLL_CLASS_IMPORT void	GetString( char* pString, int nMaxChars = 0 );
	DLL_CLASS_IMPORT void	GetString( CBufferString *pString );
	DLL_CLASS_IMPORT bool	Get( void* pMem, int size );
	DLL_CLASS_IMPORT void	GetLine( char* pLine, int nMaxChars = 0 );
	DLL_CLASS_IMPORT void	GetLine( CBufferString *pLine );

	// This will get at least 1 byte and up to nSize bytes. 
	// It will return the number of bytes actually read.
	DLL_CLASS_IMPORT int	GetUpTo( void *pMem, int nSize );

	// This version of GetString converts \" to \\ and " to \, etc.
	// It also reads a " at the beginning and end of the string
	DLL_CLASS_IMPORT void	GetDelimitedString( CUtlCharConversion *pConv, char *pString, int nMaxChars = 0 );
	DLL_CLASS_IMPORT void	GetDelimitedString( CUtlCharConversion *pConv, CBufferString *pString );
	DLL_CLASS_IMPORT char	GetDelimitedChar( CUtlCharConversion *pConv );

	// This will return the # of characters of the string about to be read out
	// NOTE: The count will *include* the terminating 0!!
	// In binary mode, it's the number of characters until the next 0
	// In text mode, it's the number of characters until the next space.
	DLL_CLASS_IMPORT int	PeekStringLength();

	// This version of PeekStringLength converts \" to \\ and " to \, etc.
	// It also reads a " at the beginning and end of the string
	// NOTE: The count will *include* the terminating 0!!
	// In binary mode, it's the number of characters until the next 0
	// In text mode, it's the number of characters between "s (checking for \")
	// Specifying false for bActualSize will return the pre-translated number of characters
	// including the delimiters and the escape characters. So, \n counts as 2 characters when bActualSize == false
	// and only 1 character when bActualSize == true
	DLL_CLASS_IMPORT int	PeekDelimitedStringLength( CUtlCharConversion *pConv, bool bActualSize = true );

	// Just like scanf, but doesn't work in binary mode
	DLL_CLASS_IMPORT int	VaScanf( const char* pFmt, va_list list );

	// Eats white space, advances Get index
	DLL_CLASS_IMPORT void	EatWhiteSpace();

	// Eats C++ style comments
	DLL_CLASS_IMPORT bool	EatCPPComment();
	DLL_CLASS_IMPORT bool	EatCComment( int * );

	// (For text buffers only)
	// Parse a token from the buffer:
	// Grab all text that lies between a starting delimiter + ending delimiter
	// (skipping whitespace that leads + trails both delimiters).
	// If successful, the get index is advanced and the function returns true,
	// otherwise the index is not advanced and the function returns false.
	DLL_CLASS_IMPORT bool	ParseToken( const char *pStartingDelim, const char *pEndingDelim, char *pString, int nMaxLen );
	DLL_CLASS_IMPORT bool	ParseToken( const char *pStartingDelim, const char *pEndingDelim, CBufferString *pString );

	// Advance the get index until after the particular string is found
	// Do not eat whitespace before starting. Return false if it failed
	// String test is case-insensitive.
	DLL_CLASS_IMPORT bool	GetToken( const char *pToken );

	// Parses the next token, given a set of character breaks to stop at
	// Returns the length of the token parsed in bytes (-1 if none parsed)
	DLL_CLASS_IMPORT int	ParseToken( const characterset_t *pBreaks, char *pTokenBuf, int nMaxLen, bool bParseComments = true );
	DLL_CLASS_IMPORT int	ParseToken( const characterset_t *pBreaks, CBufferString *pTokenBuf, bool bParseComments = true );

	// Write stuff in
	// Binary mode: it'll just write the bits directly in, and strings will be
	//		written with a null terminating character
	// Text mode: it'll convert the numbers to text versions
	//		PutString will not write a terminating character
	void			PutChar( char c );
	void			PutUnsignedChar( unsigned char uc );
	void			PutShort( short s );
	void			PutUnsignedShort( unsigned short us );
	void			PutInt( int i );
	void			PutInt64( int64_t i );
	void			PutUnsignedInt( unsigned int u );
	void			PutFloat( float f );
	void			PutDouble( double d );
	void			PutPtr( void * ); // Writes the pointer, not the pointed to
	DLL_CLASS_IMPORT void	PutString( const char* pString );
	DLL_CLASS_IMPORT void	Put( const void* pMem, int size );

	// This version of PutString converts \ to \\ and " to \", etc.
	// It also places " at the beginning and end of the string
	DLL_CLASS_IMPORT void	PutDelimitedString( CUtlCharConversion *pConv, const char *pString );
	DLL_CLASS_IMPORT void	PutDelimitedChar( CUtlCharConversion *pConv, char c );

	DLL_CLASS_IMPORT bool	PutFileContent( const char *pString );
	DLL_CLASS_IMPORT void	PutJSONString( const char *pString );

	DLL_CLASS_IMPORT void	PutZeros( int nCount );

	// Just like printf, writes a terminating zero in binary mode
	DLL_CLASS_IMPORT void	Printf( const char* pFmt, ... ) FMTFUNCTION( 2, 3 );
	DLL_CLASS_IMPORT void	VaPrintf( const char* pFmt, va_list list );

	// What am I writing (put)/reading (get)?
	void* PeekPut( int offset = 0 );
	const void* PeekGet( int offset = 0 ) const;
	DLL_CLASS_IMPORT const void* PeekGet( int nMaxSize, int nOffset );

	// Where am I writing (put)/reading (get)?
	int TellPut( ) const;
	int TellGet( ) const;

	// What's the most I've ever written?
	int TellMaxPut( ) const;

	// How many bytes remain to be read?
	// NOTE: This is not accurate for streaming text files; it overshoots
	int GetBytesRemaining() const;

	// Change where I'm writing (put)/reading (get)
	DLL_CLASS_IMPORT void SeekPut( SeekType_t type, int offset );
	DLL_CLASS_IMPORT void SeekGet( SeekType_t type, int offset );

	// Buffer base
	const void* Base() const;
	void* Base();

	// memory allocation size, does *not* reflect size written or read,
	//	use TellPut or TellGet for that
	int Size() const;

	// Am I a text buffer?
	bool IsText() const;

	// Can I grow if I'm externally allocated?
	bool IsGrowable() const;

	// Am I valid? (overflow or underflow error), Once invalid it stays invalid
	bool IsValid() const;

	// Do I contain carriage return/linefeeds? 
	bool ContainsCRLF() const;

	// Am I read-only
	bool IsReadOnly() const;

	// Converts a buffer from a CRLF buffer to a CR buffer (and back)
	// Returns false if no conversion was necessary (and outBuf is left untouched)
	// If the conversion occurs, outBuf will be cleared.
	DLL_CLASS_IMPORT bool ConvertCRLF( CUtlBuffer &outBuf );

	// Push/pop pretty-printing tabs
	void PushTab();
	void PopTab();

	// Temporarily disables pretty print
	void EnableTabs( bool bEnable );

protected:
	// error flags
	enum
	{
		PUT_OVERFLOW = 0x1,
		GET_OVERFLOW = 0x2,
		MAX_ERROR_FLAG = GET_OVERFLOW,
	};

	DLL_CLASS_IMPORT void SetOverflowFuncs( UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc );

	DLL_CLASS_IMPORT bool OnPutOverflow( int nSize );
	DLL_CLASS_IMPORT bool OnGetOverflow( int nSize );

protected:
	// Checks if a get/put is ok
	DLL_CLASS_IMPORT bool CheckPut( int size );
	DLL_CLASS_IMPORT bool CheckGet( int size );

	DLL_CLASS_IMPORT void AddNullTermination( );

	// Methods to help with pretty-printing
	bool WasLastCharacterCR();
	void PutTabs();

	// Help with delimited stuff
	DLL_CLASS_IMPORT char GetDelimitedCharInternal( CUtlCharConversion *pConv );
	DLL_CLASS_IMPORT void PutDelimitedCharInternal( CUtlCharConversion *pConv, char c );

	// Default overflow funcs
	DLL_CLASS_IMPORT bool PutOverflow( int nSize );
	DLL_CLASS_IMPORT bool GetOverflow( int nSize );

	// Does the next bytes of the buffer match a pattern?
	DLL_CLASS_IMPORT bool	PeekStringMatch( int nOffset, const char *pString, int nLen );

	// Peek size of line to come, check memory bound
	DLL_CLASS_IMPORT int	PeekLineLength();

	// How much whitespace should I skip?
	DLL_CLASS_IMPORT int	PeekWhiteSpace( int nOffset );

	// Checks if a peek get is ok
	DLL_CLASS_IMPORT bool	CheckPeekGet( int nOffset, int nSize );

public:
	// Call this to peek arbitrarily long into memory. It doesn't fail unless
	// it can't read *anything* new
	DLL_CLASS_IMPORT bool	CheckArbitraryPeekGet( int nOffset, int &nIncrement );

protected:

	template <typename T> void GetType( T& dest );
	template <typename T> void GetTypeBin( T& dest );
	template <typename T> bool GetTypeText( T &value, int nRadix = 10 );

	template <typename T> void PutType( T src );
	template <typename T> void PutTypeBin( T src );

	CUtlLeanVector<unsigned char> m_Memory;
	int m_Get;
	int m_Put;

	unsigned char m_Error;
	unsigned char m_Flags;
	unsigned char m_Reserved;

	int m_nTab;
	int m_nMaxPut;
	int m_nOffset;

	UtlBufferOverflowFunc_t m_GetOverflowFunc;
	UtlBufferOverflowFunc_t m_PutOverflowFunc;

	CByteswap	m_Byteswap;
};

constexpr CUtlBuffer::BufferFlags_t operator|( const CUtlBuffer::BufferFlags_t f, const CUtlBuffer::BufferFlags_t v )
{
	return static_cast<CUtlBuffer::BufferFlags_t>(static_cast<int>(f) | static_cast<int>(v));
}

#endif