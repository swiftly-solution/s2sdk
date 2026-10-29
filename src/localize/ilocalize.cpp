#include "public/localize/ilocalize.h"

int32 LocalizeStringIndex_t:: GetRaw() const
{ return m_Value; }

void LocalizeStringIndex_t:: SetRaw( int32 nValue )
{ m_Value = nValue; }

bool LocalizeStringIndex_t::operator==( const LocalizeStringIndex_t &other ) const
{ return m_Value == other.m_Value; }

bool LocalizeStringIndex_t::operator!=( const LocalizeStringIndex_t &other ) const
{ return m_Value != other.m_Value; }

int32 LocalizeInstanceIndex_t:: GetRaw() const
{ return m_Value; }

void LocalizeInstanceIndex_t:: SetRaw( int32 nValue )
{ m_Value = nValue; }

bool LocalizeInstanceIndex_t::operator==( const LocalizeInstanceIndex_t &other ) const
{ return m_Value == other.m_Value; }

bool LocalizeInstanceIndex_t::operator!=( const LocalizeInstanceIndex_t &other ) const
{ return m_Value != other.m_Value; }

ILocalize::~ILocalize()
{}
