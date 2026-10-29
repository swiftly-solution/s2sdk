#include "public/tier0/unicode.h"

const char * V_UnicodeAdvance( const char *pUTF8, int nCharacters )
{ return V_UnicodeAdvance( (char *)pUTF8, nCharacters ); }

const uchar16 * V_UnicodeAdvance( const uchar16 *pUTF16, int nCharacters )
{ return V_UnicodeAdvance( (uchar16 *)pUTF16, nCharacters ); }

const uchar32 * V_UnicodeAdvance( const uchar32 *pUTF32, int nCharacters )
{ return V_UnicodeAdvance( (uchar32 *)pUTF32, nCharacters ); }

void V_UnicodeTruncate( char *pUTF8, int nCharacters )
{ *V_UnicodeAdvance( pUTF8, nCharacters ) = 0; }

void V_UnicodeTruncate( uchar16 *pUTF16, int nCharacters )
{ *V_UnicodeAdvance( pUTF16, nCharacters ) = 0; }

void V_UnicodeTruncate( uchar32 *pUTF32, int nCharacters )
{ *V_UnicodeAdvance( pUTF32, nCharacters ) = 0; }

CStrAutoEncode::CStrAutoEncode( const char *buf )
{
	InitEmpty();

	m_pch = buf;
	m_bHasUTF8 = true;
}

CStrAutoEncode::CStrAutoEncode( const uchar16 *buf )
{
	InitEmpty();

	m_char16 = buf;
	m_bHasUTF16 = true;
}

CStrAutoEncode::CStrAutoEncode( const uchar32 *buf )
{
	InitEmpty();

	m_char32 = buf;
	m_bHasUTF32 = true;
}

CStrAutoEncode::CStrAutoEncode( const CStrAutoEncode &other )
{
	Copy( other );
}

const char * CStrAutoEncode::ToUTF8()
{ return ToString(); }

const char* CStrAutoEncode::ToString()
{
	PopulateUTF8();
	return m_pch;
}

const uchar16 * CStrAutoEncode::ToUTF16()
{ return ToUChar16(); }

const uchar16 * CStrAutoEncode::ToUChar16()
{
	PopulateUTF16();
	return m_char16;
}

const uchar32 * CStrAutoEncode::ToUTF32()
{ return ToUChar32(); }

const uchar32 * CStrAutoEncode::ToUChar32()
{
	PopulateUTF32();
	return m_char32;
}

CStrAutoEncode::~CStrAutoEncode()
{
	Clear();
}
