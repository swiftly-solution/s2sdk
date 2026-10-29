#include "public/entity2/gametime.h"

GameTime_t::GameTime_t( float value )
: m_Value( value )
{}

float GameTime_t::GetTime() const
{ return m_Value; }

void GameTime_t::SetTime( float value )
{ m_Value = value; }

bool GameTime_t::operator==( const GameTime_t &other ) const
{ return m_Value == other.m_Value; }
