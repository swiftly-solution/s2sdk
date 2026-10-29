#include "public/schemasystem/atomictypes.h"

bool AtomicTypeInfo_T_t::operator<( const AtomicTypeInfo_T_t& rhs ) const
{ 
	if ( m_nAtomicID != rhs.m_nAtomicID )
		return m_nAtomicID < rhs.m_nAtomicID;
	
	if ( m_pTemplateType != rhs.m_pTemplateType )
		return m_pTemplateType < rhs.m_pTemplateType;
	
	return (void *)m_pfnManipulator < (void *)rhs.m_pfnManipulator;
}

bool AtomicTypeInfo_CollectionOfT_t::operator<( const AtomicTypeInfo_CollectionOfT_t &rhs ) const
{
	if(m_nAtomicID != rhs.m_nAtomicID)
		return m_nAtomicID < rhs.m_nAtomicID;

	if(m_pTemplateType != rhs.m_pTemplateType)
		return m_pTemplateType < rhs.m_pTemplateType;

	if(m_nFixedBufferCount != rhs.m_nFixedBufferCount)
		return m_nFixedBufferCount < rhs.m_nFixedBufferCount;

	return (void *)m_pfnManipulator < (void *)rhs.m_pfnManipulator;
}

bool AtomicTypeInfo_TT_t::operator<( const AtomicTypeInfo_TT_t& rhs ) const
{ 
	if ( m_nAtomicID != rhs.m_nAtomicID )
		return m_nAtomicID < rhs.m_nAtomicID;
	
	if ( m_pTemplateType != rhs.m_pTemplateType )
		return m_pTemplateType < rhs.m_pTemplateType;
	
	return m_pTemplateType2 < rhs.m_pTemplateType2;
}

bool AtomicTypeInfo_I_t::operator<( const AtomicTypeInfo_I_t& rhs ) const
{ 
	if ( m_nAtomicID != rhs.m_nAtomicID )
		return m_nAtomicID < rhs.m_nAtomicID;

	return m_nInteger < rhs.m_nInteger;
}

bool TypeAndCountInfo_t::operator<( const TypeAndCountInfo_t& rhs ) const
{ 
	if ( m_nElementCount != rhs.m_nElementCount )
		return m_nElementCount < rhs.m_nElementCount;
	
	return m_pElementType < rhs.m_pElementType;
}
