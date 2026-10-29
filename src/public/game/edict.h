//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef EDICT_H
#define EDICT_H

#ifdef _WIN32
#pragma once
#endif

#include "public/const.h"

struct edict_t;

#define FL_EDICT_CHANGED	(1<<0)	// Game DLL sets this when the entity state changes
// Mutually exclusive with FL_EDICT_PARTIAL_CHANGE.

// This is used internally to edict_t to remember that it's carrying a 
// "full change list" - all its properties might have changed their value.
#define FL_FULL_EDICT_CHANGED		(1<<1)

#define FL_EDICT_FULLCHECK	(0<<0)  // call ShouldTransmit() each time, this is a fake flag
#define FL_EDICT_ALWAYS		(1<<2)	// always transmit this entity
#define FL_EDICT_DONTSEND	(1<<3)	// don't transmit this entity
#define FL_EDICT_PVSCHECK	(1<<4)	// always transmit entity, but cull against PVS

#endif // EDICT_H