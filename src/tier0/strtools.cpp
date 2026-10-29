#include "public/tier0/strtools.h"

void  V_strcat( INOUT_Z_CAP( cchDest ) char *dest, const char *src, int cchDest )
{
	_V_strncat( dest, src, (int)cchDest, COPY_ALL_CHARACTERS );
}

void  V_wcscat( INOUT_Z_CAP(cchDest) wchar_t *dest, const wchar_t *src, int cchDest )
{
	_V_wcsncat_cch( dest, src, cchDest, COPY_ALL_CHARACTERS );
}

bool  V_isdigit( char c )
{
	return c >= '0' && c <= '9';
}

bool  V_isempty( const char* pszString )
{ return !pszString || !pszString[ 0 ]; }

bool  V_isalpha(char c)
{ return isalpha( (unsigned char)c ) != 0; }

bool  V_isalnum(char c)
{ return isalnum( (unsigned char)c ) != 0; }

bool  V_isprint(char c)
{ return isprint( (unsigned char)c ) != 0; }

bool  V_isxdigit(char c)
{ return isxdigit( (unsigned char)c ) != 0; }

bool  V_ispunct(char c)
{ return ispunct( (unsigned char)c ) != 0; }

bool  V_isgraph(char c)
{ return isgraph( (unsigned char)c ) != 0; }

bool  V_isupper(char c)
{ return isupper( (unsigned char)c ) != 0; }

bool  V_islower(char c)
{ return islower( (unsigned char)c ) != 0; }

bool  V_iscntrl(char c)
{ return iscntrl( (unsigned char)c ) != 0; }

bool  V_isspace(char c)
{ return isspace( (unsigned char)c ) != 0; }

bool 	V_StringHasPrefix             ( const char *str, const char *prefix )
{ return V_StringAfterPrefix( str, prefix ) != NULL; }

bool 	V_StringHasPrefixCaseSensitive( const char *str, const char *prefix )
{ return V_StringAfterPrefixCaseSensitive( str, prefix ) != NULL; }

char CUtlCharConversion:: GetEscapeChar() const
{ return m_nEscapeChar; }

const char * CUtlCharConversion::GetDelimiter() const
{ return m_pDelimiter; }

int CUtlCharConversion:: GetDelimiterLength() const
{ return m_nDelimiterLength; }

const char * CUtlCharConversion::GetConversionString( char c ) const
{ return m_pReplacements[(unsigned char)c].m_pReplacementString; }

int CUtlCharConversion:: GetConversionLength( char c ) const
{ return m_pReplacements[(unsigned char)c].m_nLength; }

int CUtlCharConversion:: MaxConversionLength() const
{ return m_nMaxConversionLength; }
