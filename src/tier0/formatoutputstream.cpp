#include "public/tier0/formatoutputstream.h"

CFormatStringElement::CFormatStringElement()
: m_unk001( 0 )
{}

CFormatStringElement::operator const char *() const
{ return m_szElement; }

IFormatOutputStream::IFormatOutputStream()
: m_nFlags( DEFAULT ), m_nCharsWritten( 0 ), m_unk101( 64 ), m_unk102( 1 ), m_unk103( INT_MAX )
{}

IFormatOutputStream::~IFormatOutputStream()
{}

bool IFormatOutputStream::UnicodeOK() const
{ return ( m_nFlags & UNICODE_OK ) != 0; }

bool IFormatOutputStream::IsHTML() const
{ return ( m_nFlags & HTML ) != 0; }

bool IFormatOutputStream::IsMultiLine() const
{ return ( m_nFlags & MULTI_LINE ) != 0; }

bool IFormatOutputStream::IsNonFull() const
{ return !IsFull(); }

void IFormatOutputStream::NewLineAndIndent()
{ PutC( '\n' ); OutputIndentation(); }

int IFormatOutputStream::GetCharsWritten() const
{ return m_nCharsWritten; }

CBufferStringFormatOutputStream::CBufferStringFormatOutputStream()
: m_pString( nullptr )
{}

CBufferStringFormatOutputStream::CBufferStringFormatOutputStream( CBufferString &str )
: m_pString( &str )
{}

void CBufferStringFormatOutputStream::SetString( CBufferString &str )
{ m_pString = &str; }

CCharBufferFormatOutputStream::CCharBufferFormatOutputStream( char *pBuffer, int nBufferSize )
: m_pCurrent( pBuffer ), m_pEnd( pBuffer + nBufferSize )
{}
