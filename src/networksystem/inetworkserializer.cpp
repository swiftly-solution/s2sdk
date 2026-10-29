#include "public/networksystem/inetworkserializer.h"

SerializerFieldLookup_t::SerializerFieldLookup_t( const char *fieldname )
: m_FieldName( fieldname ), m_FieldIndex( 0 )
{}

CNetworkSerializerFieldInfo * CNetworkSerializerClassInfo::FindField( const char *field_name ) const
{
	auto handle = m_FieldLookupTable.Find( field_name );

	if(handle != m_FieldLookupTable.InvalidHandle())
	{
		auto elem = m_FieldLookupTable.Element( handle );
		Assert( elem.m_FieldIndex >= 0 && elem.m_FieldIndex < m_nTotalFieldEntries );
		return m_Fields[elem.m_FieldIndex];
	}

	return nullptr;
}
