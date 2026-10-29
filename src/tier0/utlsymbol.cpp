#include "public/tier0/utlsymbol.h"

CUtlSymbol::CUtlSymbol()
: m_Id(UTL_INVAL_SYMBOL)
{}

CUtlSymbol::CUtlSymbol( UtlSymId_t id )
: m_Id(id)
{}

CUtlSymbol::CUtlSymbol( CUtlSymbol const& sym )
: m_Id(sym.m_Id)
{}

CUtlSymbol& CUtlSymbol::operator=( CUtlSymbol const& src )
{ m_Id = src.m_Id; return *this; }

bool CUtlSymbol::operator==( CUtlSymbol const& src ) const
{ return m_Id == src.m_Id; }

bool CUtlSymbol::IsValid() const
{ return m_Id != UTL_INVAL_SYMBOL; }

CUtlSymbol::operator UtlSymId_t () const
{ return m_Id; }
