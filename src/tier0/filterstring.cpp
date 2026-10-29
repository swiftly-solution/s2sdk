#include "public/tier0/filterstring.h"

const char * CFilterString::GetString() const
{ return m_InputString.Get(); }

FilterStringOp_t CFilterString:: GetOp() const
{ return m_Op; }

FilterStringCase_t CFilterString:: GetCase() const
{ return m_Case; }
