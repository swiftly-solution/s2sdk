#include "public/game/ehandle.h"

CEntityInstance* CEntityHandle::Get() const
{
	return GameEntitySystem()->GetEntityInstance( *this );
}

CEntityHandle CEntityHandle::FromPackedInt( uint32 packed_int_handle )
{
	if(packed_int_handle == 0xFFFFFF)
		return CEntityHandle();

	CEntityIndex index = packed_int_handle & 0x3FFF;
	int serial = (packed_int_handle >> 14) & 0x3FF;

	auto entity = GameEntitySystem()->GetEntityInstance( index );
	if(!entity)
		return CEntityHandle();

	auto ent_handle = entity->GetRefEHandle();

	// Since we don't have the full serial part, validate the one we have
	if((ent_handle.GetSerialNumber() & 0x3FF) != serial)
		return CEntityHandle();

	return ent_handle;
}
