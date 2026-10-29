//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ENTITYHANDLE_H
#define ENTITYHANDLE_H
#ifdef _WIN32
#pragma once
#endif

#include "public/const.h"

class CEntityInstance;

class CEntityHandle
{
public:
	friend class CEntityIdentity;

	CEntityHandle();
	CEntityHandle(uint32 value);
	CEntityHandle(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Term();

	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	uint32 ToInt() const;

	// AMNOTE: Packs handle to an int used in net messages for example, replicates Server_EHandleToInt logic
	uint32 ToPackedInt() const;
	// AMNOTE: Unpacks previously packed handle, mostly used in net messages.
	// Note: this is implemented in game code (ehandle.h)
	static CEntityHandle FromPackedInt( uint32 packed_handle );

	bool operator !=(const CEntityHandle& other) const;
	bool operator ==(const CEntityHandle& other) const;
	bool operator ==(const CEntityInstance* pEnt) const;
	bool operator !=(const CEntityInstance* pEnt) const;
	bool operator <(const CEntityHandle& other) const;
	bool operator <(const CEntityInstance* pEnt) const;

	// Assign a value to the handle.
	const CEntityHandle& operator=(const CEntityInstance* pEntity);
	const CEntityHandle& Set(const CEntityInstance* pEntity);

	// Use this to dereference the handle.
	// Note: this is implemented in game code (ehandle.h)
	CEntityInstance* Get() const;

protected:
	union
	{
		uint32 m_Index;
		struct
		{
			uint32 m_EntityIndex : 15;
			uint32 m_Serial : 17;
		} m_Parts;
	};
};

typedef CEntityHandle CBaseHandle;

#endif // ENTITYHANDLE_H
