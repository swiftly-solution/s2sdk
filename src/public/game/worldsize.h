//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
// worldsize.h  -- extent of world and resolution/size of coordinate messages used in engine

#ifndef WORLDSIZE_H
#define WORLDSIZE_H
#pragma once

#include "public/tier0/platform.h"

class KeyValues;

// These definitions must match the coordinate message sizes in coordsize.h

// Following values should be +16384, -16384, +15/16, -15/16
// NOTE THAT IF THIS GOES ANY BIGGER THEN DISK NODES/LEAVES CANNOT USE SHORTS TO STORE THE BOUNDS
#define MAX_COORD_INTEGER			(16384)
#define MIN_COORD_INTEGER			(-MAX_COORD_INTEGER)
#define MAX_COORD_FRACTION			(1.0-(1.0/16.0))
#define MIN_COORD_FRACTION			(-1.0+(1.0/16.0))

#define MAX_COORD_FLOAT				(16384.0f)
#define MIN_COORD_FLOAT				(-MAX_COORD_FLOAT)

// Width of the coord system, which is TOO BIG to send as a client/server coordinate value
#define COORD_EXTENT				(2*MAX_COORD_INTEGER)

// Maximum traceable distance ( assumes cubic world and trace from one corner to opposite )
// COORD_EXTENT * sqrt(3)
#define MAX_TRACE_LENGTH			( 1.732050807569 * COORD_EXTENT )		

// This value is the LONGEST possible range (limited by max valid coordinate number, not 2x)
#define MAX_COORD_RANGE				(MAX_COORD_INTEGER)

#define TEST_COORD( v )				(((v).x>=MIN_COORD_INTEGER*2) && ((v).x<=MAX_COORD_INTEGER*2) && \
									 ((v).y>=MIN_COORD_INTEGER*2) && ((v).y<=MAX_COORD_INTEGER*2) && \
									 ((v).z>=MIN_COORD_INTEGER*2) && ((v).z<=MAX_COORD_INTEGER*2))

#define ASSERT_COORD( v )			Assert( TEST_COORD( v ) );

// The world extent the game runs with: gameinfo.gi's Engine2/MapMaxCoord (MAX_COORD_FLOAT when not set),
// or the one a server set for the session
PLATFORM_INTERFACE float g_flConfigMaxCoord;
PLATFORM_INTERFACE int g_nConfigMaxCoord;
// gameinfo.gi's Engine2/MaxObjectPosition, 1000000 when not set
PLATFORM_INTERFACE float g_flConfigMaxObjectPosition;
// gameinfo.gi's Hammer/GameDefaultFarZ, 4 * g_flConfigMaxCoord when not set
PLATFORM_INTERFACE float g_flToolsDefaultFarZ;

// Reads the values above from gameinfo.gi
PLATFORM_INTERFACE void Worldsize_Init( KeyValues *pGameInfo );
// Gameinfo.gi's world extent, which the session one overrides
PLATFORM_INTERFACE float Worldsize_GetGameInfoMaxCoordFloat();
// Returns false, keeping the current extent, unless 0 < flMaxCoord <= 2^24
PLATFORM_INTERFACE bool Worldsize_SetSessionMaxCoord( float flMaxCoord );
// Goes back to gameinfo.gi's world extent
PLATFORM_INTERFACE void Worldsize_ClearSessionMaxCoord();

#endif // WORLDSIZE_H
