#include "public/tier0/bitstring.h"

bool CVariableBitStringBase::IsFixedSize() const
{ return false; }

int CVariableBitStringBase::Size() const
{ return m_numBits; }

int CVariableBitStringBase::GetNumInts() const
{ return m_numInts; }

int * CVariableBitStringBase::GetInts()
{ return m_pInt; }

const int * CVariableBitStringBase::GetInts() const
{ return m_pInt; }

CVariableBitStringBase::CVariableBitStringBase()
: m_numBits( 0 ), m_numInts( 0 ), m_iBitStringStorage( 0 ), m_pInt( nullptr )
{}

CVariableBitStringBase::~CVariableBitStringBase()
{ FreeInts(); }
