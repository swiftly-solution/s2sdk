#ifndef _ENTITYHANDLE_H
#define _ENTITYHANDLE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform_base.h"
#include "public/const.h"

class CEntityInstance;

class CEntityHandle
{
public:
	friend class CEntityIdentity;

    CEntityHandle();
	CEntityHandle(uint32_t value);
	CEntityHandle(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Term();

	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	uint32_t ToInt() const;
    uint32_t ToPackedInt() const;

    static CEntityHandle FromPackedInt( uint32_t packed_handle );

    bool operator !=(const CEntityHandle& other) const;
	bool operator ==(const CEntityHandle& other) const;
	bool operator ==(const CEntityInstance* pEnt) const;
	bool operator !=(const CEntityInstance* pEnt) const;
	bool operator <(const CEntityHandle& other) const;
	bool operator <(const CEntityInstance* pEnt) const;

    const CEntityHandle& operator=(const CEntityInstance* pEntity);
	const CEntityHandle& Set(const CEntityInstance* pEntity);

    CEntityInstance* Get() const;
protected:
	union
	{
		uint32_t m_Index;
		struct
		{
			uint32_t m_EntityIndex : 15;
			uint32_t m_Serial : 17;
		} m_Parts;
	};
};

#endif