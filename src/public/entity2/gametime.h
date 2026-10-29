#ifndef GAME_TIME_H
#define GAME_TIME_H

#ifdef _WIN32
#pragma once
#endif

struct GameTime_t
{
public:
	GameTime_t( float value = 0.0f );

	float GetTime() const;
	void SetTime( float value );

	bool operator==( const GameTime_t &other ) const;

private:
	float m_Value;
};

#endif