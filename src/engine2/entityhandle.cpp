#include "public/engine2/entityhandle.h"

inline CEntityHandle::CEntityHandle()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

inline CEntityHandle::CEntityHandle(uint32_t value)
{
	m_Index = value;
}

inline CEntityHandle::CEntityHandle(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}

inline void CEntityHandle::Init(int iEntry, int iSerialNumber)
{
	m_Parts.m_EntityIndex = iEntry;
	m_Parts.m_Serial = iSerialNumber;
}

inline void CEntityHandle::Term()
{
	m_Index = INVALID_EHANDLE_INDEX;
}

inline bool CEntityHandle::IsValid() const
{
	return m_Index != INVALID_EHANDLE_INDEX;
}

inline int CEntityHandle::GetEntryIndex() const
{
	if (IsValid())
	{
		return m_Parts.m_EntityIndex;
	}

	return -1;
}

inline int CEntityHandle::GetSerialNumber() const
{
	return m_Parts.m_Serial;
}

inline uint32_t CEntityHandle::ToInt() const
{
	return m_Index;
}

inline uint32_t CEntityHandle::ToPackedInt() const
{
	if(!IsValid())
		return 0xFFFFFF;

	return GetEntryIndex() | ((GetSerialNumber() & 0x3FF) << 14);
}

inline bool CEntityHandle::operator !=(const CEntityHandle& other) const
{
	return m_Index != other.m_Index;
}

inline bool CEntityHandle::operator ==(const CEntityHandle& other) const
{
	return m_Index == other.m_Index;
}

inline bool CEntityHandle::operator ==(const CEntityInstance* pEnt) const
{
	return Get() == pEnt;
}

inline bool CEntityHandle::operator !=(const CEntityInstance* pEnt) const
{
	return Get() != pEnt;
}

inline bool CEntityHandle::operator <(const CEntityHandle& other) const
{
	return m_Index < other.m_Index;
}

inline const CEntityHandle &CEntityHandle::operator=( const CEntityInstance *pEntity )
{
	return Set( pEntity );
}