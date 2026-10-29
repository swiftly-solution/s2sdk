#include "public/tier0/utlstringtoken.h"

CUtlStringToken::CUtlStringToken(uint32_t hashCode)
: m_nHashCode(hashCode)
{}

CUtlStringToken::CUtlStringToken(const char* str)
{ *this = Make(str); }

bool CUtlStringToken::operator==( CUtlStringToken const &other ) const
{ return ( other.m_nHashCode == m_nHashCode ); }

bool CUtlStringToken::operator!=( CUtlStringToken const &other ) const
{ return !operator==( other ); }

bool CUtlStringToken::operator<( CUtlStringToken const &other ) const
{ return ( m_nHashCode < other.m_nHashCode ); }

bool CUtlStringToken::IsValid() const
{ return m_nHashCode != 0; }

uint32_t CUtlStringToken::GetHashCode() const
{ return m_nHashCode; }

void CUtlStringToken::SetHashCode( uint32_t hash )
{ m_nHashCode = hash; }
