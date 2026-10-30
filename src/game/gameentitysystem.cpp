#include "public/game/gameentitysystem.h"

void IEntityListener::OnEntityCreated(CEntityInstance* pEntity)
{}

void IEntityListener::OnEntitySpawned(CEntityInstance* pEntity)
{}

void IEntityListener::OnEntityDeleted(CEntityInstance* pEntity)
{}

void IEntityListener::OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent)
{}
