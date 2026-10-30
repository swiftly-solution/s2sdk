#include "public/entity2/entityhandle.h"
#include "public/entity2/entityinstance.h"
#include "public/entity2/entitysystem.h"


CEntityHandle::CEntityHandle()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

CEntityHandle::CEntityHandle(uint32 value)
{
	m_Index = value;
}

CEntityHandle::CEntityHandle(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}

void CEntityHandle::Init(int iEntry, int iSerialNumber)
{
	m_Parts.m_EntityIndex = iEntry;
	m_Parts.m_Serial = iSerialNumber;
}

void CEntityHandle::Term()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

bool CEntityHandle::IsValid() const
{
	return m_Index != INVALID_EHANDLE_INDEX;
}

int CEntityHandle::GetEntryIndex() const
{
	if (IsValid())
	{
		return m_Parts.m_EntityIndex;
	}

	return -1;
}

int CEntityHandle::GetSerialNumber() const
{
	return m_Parts.m_Serial;
}

uint32 CEntityHandle::ToInt() const
{
	return m_Index;
}

uint32 CEntityHandle::ToPackedInt() const
{
	if(!IsValid())
		return 0xFFFFFF;

	return GetEntryIndex() | ((GetSerialNumber() & 0x3FF) << 14);
}

bool CEntityHandle::operator !=(const CEntityHandle& other) const
{
	return m_Index != other.m_Index;
}

bool CEntityHandle::operator ==(const CEntityHandle& other) const
{
	return m_Index == other.m_Index;
}

bool CEntityHandle::operator ==(const CEntityInstance* pEnt) const
{
	return Get() == pEnt;
}

bool CEntityHandle::operator !=(const CEntityInstance* pEnt) const
{
	return Get() != pEnt;
}

bool CEntityHandle::operator <(const CEntityHandle& other) const
{
	return m_Index < other.m_Index;
}

const CEntityHandle &CEntityHandle::operator=( const CEntityInstance *pEntity )
{
	return Set( pEntity );
}

bool CEntityHandle::operator <(const CEntityInstance* pEnt) const
{
	return Get() < pEnt;
}

const CEntityHandle &CEntityHandle::Set( const CEntityInstance *pEntity )
{
	if ( pEntity )
		m_Index = pEntity->GetRefEHandle().m_Index;
	else
		m_Index = INVALID_EHANDLE_INDEX;

	return *this;
}
