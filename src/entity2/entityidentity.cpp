#include "public/entity2/entityidentity.h"

CEntityHandle CEntityIdentity:: GetRefEHandle() const
{
	CEntityHandle handle = m_EHandle;
	handle.m_Parts.m_Serial -= (m_flags & EF_IS_INVALID_EHANDLE);

	return handle;
}

const char * CEntityIdentity::GetClassname() const
{
	return m_designerName.String();
}

CEntityIndex CEntityIdentity:: GetEntityIndex() const
{
	return m_EHandle.GetEntryIndex();
}

SpawnGroupHandle_t CEntityIdentity:: GetSpawnGroup() const
{
	return m_hSpawnGroup;
}
