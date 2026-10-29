#include "public/entity2/entitysystem.h"

unsigned int CEntitySystem::MurmurHash2HashFunctor::operator()( uint32 n ) const
{ return MurmurHash2( &n, sizeof( uint32 ), 0x3501A674 ); }

CEntityInstance * CEntitySystem::GetEntityInstance( CEntityIdentity *ident )
{ return ident ? ident->m_pInstance : nullptr; }

CEntityInstance * CEntitySystem::GetEntityInstance( CEntityIndex entnum )
{ return GetEntityInstance( GetEntityIdentity( entnum ) ); }

CEntityInstance * CEntitySystem::GetEntityInstance( const CEntityHandle &hEnt )
{ return GetEntityInstance( GetEntityIdentity( hEnt ) ); }

CEntityInstance * CEntitySystem::GetPlayerController( CPlayerSlot slot )
{ return slot.IsValid() ? GetEntityInstance( CEntityIndex( slot.Get() + 1 ) ) : nullptr; }

void CEntitySystem:: AddEntityKeyValuesAllocatorRef()
{ ++m_nEntityKeyValuesAllocatorRefCount; }

void CEntitySystem:: ReleaseEntityKeyValuesAllocatorRef()
{
	if (--m_nEntityKeyValuesAllocatorRefCount == 0) 
		m_EntityKeyValuesAllocator.Clear();
}

CKV3Arena* CEntitySystem:: GetEntityKeyValuesAllocator()
{ return &m_EntityKeyValuesAllocator; }

void EntityInstanceIter_t:: SetWorldGroupId(WorldGroupId_t hWorldGroupId)
{ m_hWorldGroupId = hWorldGroupId; }

void EntityInstanceByNameIter_t:: SetWorldGroupId(WorldGroupId_t hWorldGroupId)
{ m_hWorldGroupId = hWorldGroupId; }

void EntityInstanceByClassIter_t:: SetWorldGroupId(WorldGroupId_t hWorldGroupId)
{ m_hWorldGroupId = hWorldGroupId; }
