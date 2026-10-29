#include "public/entity2/entitycomponent.h"

CEntityOwnerPtr::operator CEntityInstance*()
{ return operator->(); }

CEntityOwnerPtr::operator const CEntityInstance*() const
{ return operator->(); }

void CEntityOwnerPtr:: Set( CEntityInstance* pEntity )
{ CSmartPtr<CEntityInstance, CNullRefCountAccessor>::operator=( pEntity ); }
