#include "public/entity2/entityinstance.h"

NetworkStateChangedData:: NetworkStateChangedData()
: m_bPartialChange(1), m_nLine(-1), m_nArrayIndex(-1), m_nPathIndex(ChangeAccessorFieldPathIndex_t()), m_bRequiresPathIndex(false), m_unk101(false)
{ }

NetworkStateChangedData:: NetworkStateChangedData( bool bFullChanged )
:
	m_bPartialChange(static_cast<uint32_t>(!bFullChanged)), m_nLine(-1),
	m_nArrayIndex(-1), m_nPathIndex(ChangeAccessorFieldPathIndex_t()),
	m_bRequiresPathIndex(false), m_unk101(false)
{ }

NetworkStateChangedData:: NetworkStateChangedData( uint32_t nLocalOffset, int32_t nArrayIndex, ChangeAccessorFieldPathIndex_t nPathIndex )
:
	m_bPartialChange(1), m_LocalOffsets(0, 1), m_nLine(-1), m_nArrayIndex(nArrayIndex), m_nPathIndex(nPathIndex), m_bRequiresPathIndex(false), m_unk101(false)
{
	m_LocalOffsets.AddToHead(nLocalOffset);
}

NetworkStateChangedData:: NetworkStateChangedData(const std::initializer_list< uint32_t > nLocalOffsets, int32_t nArrayIndex, ChangeAccessorFieldPathIndex_t nPathIndex)
:
	m_bPartialChange(1), m_LocalOffsets(0, (int)nLocalOffsets.size()), m_nLine(-1), m_nArrayIndex(nArrayIndex), m_nPathIndex(nPathIndex), m_bRequiresPathIndex(true), m_unk101(false)
{
	for ( const uint32_t& nLocalOffset : nLocalOffsets )
	{
		m_LocalOffsets.AddToTail( nLocalOffset );
	}
}

NetworkStateChangedRemove_t:: NetworkStateChangedRemove_t()
: m_nFieldOffset(0)
{ }

CEntityHandle CEntityInstance:: GetRefEHandle() const
{
	return m_pEntity->GetRefEHandle();
}

const char * CEntityInstance::GetClassname() const
{
	return m_pEntity->GetClassname();
}

CEntityIndex CEntityInstance:: GetEntityIndex() const
{
	return m_pEntity->GetEntityIndex();
}
