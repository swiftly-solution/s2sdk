#include "public/game/gameentitysystem.h"

void IEntityListener::OnEntityCreated(CEntityInstance* pEntity)
{}

void IEntityListener::OnEntitySpawned(CEntityInstance* pEntity)
{}

void IEntityListener::OnEntityDeleted(CEntityInstance* pEntity)
{}

void IEntityListener::OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent)
{}

void CGameEntitySystem::AddListenerEntity(IEntityListener* pListener)
{
	if (m_entityListeners.Find(pListener) == -1)
		m_entityListeners.AddToTail(pListener);
}

void CGameEntitySystem::RemoveListenerEntity(IEntityListener* pListener)
{
	m_entityListeners.FindAndRemove(pListener);
}
