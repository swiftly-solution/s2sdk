#include "public/entity2/entitykeyvalues.h"
#include "public/game/gameentitysystem.h"

CUtlScratchMemoryPool *CEntityVariantAllocator::sm_pMemoryPool = nullptr;

void CEntityVariantAllocator::Free( void *pMemory )
{ /* Skipped intentionally */ }

void * CEntityVariantAllocator::Allocate( uint nSize )
{
	return sm_pMemoryPool->AllocAligned( nSize, 8 * (nSize >= 16) + 8 );
}

void CEntityVariantAllocator::Activate( CUtlScratchMemoryPool *pMemoryPool, bool bEnable )
{
	sm_pMemoryPool = bEnable ? pMemoryPool : nullptr;
}

IEntityKeyComplex::IEntityKeyComplex()
: m_nRefCount( 0 )
{}

void IEntityKeyComplex::AddRef()
{ ++m_nRefCount; }

void IEntityKeyComplex::Release()
{ if ( --m_nRefCount == 0 ) DeleteThis(); }

CEntityKeyValues::Iterator_t::Iterator_t( const KeyValues3* _keys, int _index )
: keys( _keys ), index( _index )
{}

bool CEntityKeyValues::Iterator_t::operator==( const Iterator_t it ) const
{ return keys == it.keys && index == it.index; }

bool CEntityKeyValues::Iterator_t::operator!=( const Iterator_t it ) const
{ return keys != it.keys || index != it.index; }

bool CEntityKeyValues::IsQueuedForSpawn() const
{ return m_nQueuedForSpawnCount > 0; }

bool CEntityKeyValues::IsLoggingEnabled() const
{ return m_bAllowLogging; }

void CEntityKeyValues::EnableLogging( bool bEnable )
{ m_bAllowLogging = bEnable; }

EntityIOConnectionDescFat_t* CEntityKeyValues::GetConnectionDesc( int nDesc )
{ return &m_connectionDescs[nDesc]; }

const EntityIOConnectionDescFat_t* CEntityKeyValues::GetConnectionDesc( int nDesc ) const
{ return &m_connectionDescs[nDesc]; }

int CEntityKeyValues::GetNumConnectionDescs() const
{ return m_connectionDescs.Count(); }

bool CEntityKeyValues::HasValue( const EntityKeyId_t &id ) const
{ bool bIsAttribute; return ( GetKeyValue( id, &bIsAttribute ) && !bIsAttribute ); }

bool CEntityKeyValues::HasAttribute( const EntityKeyId_t &id ) const
{ bool bIsAttribute; return ( GetKeyValue( id, &bIsAttribute ) && bIsAttribute ); }

CEntityKeyValues& CEntityKeyValues::operator=( const CEntityKeyValues& src )
{ CopyFrom( &src ); return *this; }

CEntityKeyValues::EntityComplexKeyListElem_t::EntityComplexKeyListElem_t( IEntityKeyComplex* pKey, EntityComplexKeyListElem_t* pNext )
: m_pKey( pKey ), m_pNext( pNext )
{}

CEntityKeyValues::Iterator_t CEntityKeyValues::First() const
{
	const KeyValues3* keys = NULL;

	if ( m_pAllocator )
	{
		if ( m_pValues->GetMemberCount() > 0 )
			keys = m_pValues;
		else
			keys = m_pAttributes;
	}

	return CEntityKeyValues::Iterator_t( keys, 0 );
}

CEntityKeyValues::Iterator_t CEntityKeyValues::Next( const CEntityKeyValues::Iterator_t &it ) const
{
	const KeyValues3* keys = it.keys;
	int index = it.index + 1;

	if ( index >= keys->GetMemberCount() )
	{
		if ( keys == m_pValues )
		{
			keys = m_pAttributes;
			index = 0;
		}
		else
		{
			index = keys->GetMemberCount();
		}
	}

	return CEntityKeyValues::Iterator_t( keys, index );
}

bool CEntityKeyValues::IsValidIterator( const CEntityKeyValues::Iterator_t &it ) const
{
	return ( it.keys != NULL ) && ( it.index < it.keys->GetMemberCount() );
}

const KeyValues3* CEntityKeyValues::GetKeyValue( const CEntityKeyValues::Iterator_t &it ) const
{
	if ( it.index >= it.keys->GetMemberCount() )
		return NULL;

	return it.keys->GetMember( it.index );
}

EntityKeyId_t CEntityKeyValues::GetEntityKeyId( const CEntityKeyValues::Iterator_t &it ) const
{
	if ( it.index >= it.keys->GetMemberCount() )
		return EntityKeyId_t();

	return it.keys->GetKV3MemberName( it.index );
}

const char* CEntityKeyValues::GetAttributeName( const CEntityKeyValues::Iterator_t &it ) const
{
	if ( it.keys == m_pValues )
		return NULL;

	if ( it.index >= it.keys->GetMemberCount() )
		return "<none>";
	
	return it.keys->GetMemberName( it.index );
}

bool CEntityKeyValues::IsAttribute( const CEntityKeyValues::Iterator_t &it ) const
{
	return ( it.index < it.keys->GetMemberCount() ) && ( it.keys == m_pAttributes );
}

bool CEntityKeyValues::GetBool( const EntityKeyId_t &id, bool defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetBool( defaultValue ) : defaultValue;
}

int CEntityKeyValues::GetInt( const EntityKeyId_t &id, int defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetInt( defaultValue ) : defaultValue;
}

uint CEntityKeyValues::GetUint( const EntityKeyId_t &id, uint defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetUInt() : defaultValue;
}

int64 CEntityKeyValues::GetInt64( const EntityKeyId_t &id, int64 defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetInt64( defaultValue ) : defaultValue;
}

uint64 CEntityKeyValues::GetUint64( const EntityKeyId_t &id, uint64 defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetUInt64( defaultValue ) : defaultValue;
}

float CEntityKeyValues::GetFloat( const EntityKeyId_t &id, float defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetFloat( defaultValue ) : defaultValue;
}

double CEntityKeyValues::GetDouble( const EntityKeyId_t &id, double defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetDouble( defaultValue ) : defaultValue;
}

const char* CEntityKeyValues::GetString( const EntityKeyId_t &id, const char *defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetString( defaultValue ) : defaultValue;
}

void* CEntityKeyValues::GetPtr( const EntityKeyId_t &id, void *defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetPointer( defaultValue ) : defaultValue;
}

CUtlStringToken CEntityKeyValues::GetStringToken( const EntityKeyId_t &id, CUtlStringToken defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetStringToken( defaultValue ) : defaultValue;
}

Color CEntityKeyValues::GetColor( const EntityKeyId_t &id, const Color &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetColor( defaultValue ) : defaultValue;
}

Vector CEntityKeyValues::GetVector( const EntityKeyId_t &id, const Vector &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetVector( defaultValue ) : defaultValue;
}

Vector2D CEntityKeyValues::GetVector2D( const EntityKeyId_t &id, const Vector2D &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetVector2D( defaultValue ) : defaultValue;
}

Vector4D CEntityKeyValues::GetVector4D( const EntityKeyId_t &id, const Vector4D &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetVector4D( defaultValue ) : defaultValue;
}

Quaternion CEntityKeyValues::GetQuaternion( const EntityKeyId_t &id, const Quaternion &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetQuaternion( defaultValue ) : defaultValue;
}

QAngle CEntityKeyValues::GetQAngle( const EntityKeyId_t &id, const QAngle &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetQAngle( defaultValue ) : defaultValue;
}

matrix3x4_t CEntityKeyValues::GetMatrix3x4( const EntityKeyId_t &id, const matrix3x4_t &defaultValue ) const
{
	const KeyValues3* kv = GetKeyValue( id );
	return kv ? kv->GetMatrix3x4( defaultValue ) : defaultValue;
}

void CEntityKeyValues::SetBool( const EntityKeyId_t &id, bool value, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetBool( value );
}

void CEntityKeyValues::SetInt( const EntityKeyId_t &id, int value, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetInt( value );
}

void CEntityKeyValues::SetUint( const EntityKeyId_t &id, uint value, bool bAsAttribute )
{
	KeyValues3* val = SetKeyValue( id, bAsAttribute );
	if ( val ) val->SetUInt( value );
}

void CEntityKeyValues::SetInt64( const EntityKeyId_t &id, int64 value, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetInt64( value );
}

void CEntityKeyValues::SetUint64( const EntityKeyId_t &id, uint64 value, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetUInt64( value );
}

void CEntityKeyValues::SetFloat( const EntityKeyId_t &id, float value, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetFloat( value );
}

void CEntityKeyValues::SetDouble( const EntityKeyId_t &id, double value, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetDouble( value );
}

void CEntityKeyValues::SetString( const EntityKeyId_t &id, const char* string, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) SetString( kv, string );
}

void CEntityKeyValues::SetPtr( const EntityKeyId_t &id, void* ptr, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetPointer( ptr );
}

void CEntityKeyValues::SetStringToken( const EntityKeyId_t &id, CUtlStringToken token, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetStringToken( token );
}

void CEntityKeyValues::SetEHandle( const EntityKeyId_t &id, CEntityHandle ehandle, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetEHandle( ehandle );
}

void CEntityKeyValues::SetColor( const EntityKeyId_t &id, const Color &color, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetColor( color );
}

void CEntityKeyValues::SetVector( const EntityKeyId_t &id, const Vector &vec, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetVector( vec );
}

void CEntityKeyValues::SetVector2D( const EntityKeyId_t &id, const Vector2D &vec2d, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetVector2D( vec2d );
}

void CEntityKeyValues::SetVector4D( const EntityKeyId_t &id, const Vector4D &vec4d, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetVector4D( vec4d );
}

void CEntityKeyValues::SetQuaternion( const EntityKeyId_t &id, const Quaternion &quat, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetQuaternion( quat );
}

void CEntityKeyValues::SetQAngle( const EntityKeyId_t &id, const QAngle &ang, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetQAngle( ang );
}

void CEntityKeyValues::SetMatrix3x4( const EntityKeyId_t &id, const matrix3x4_t &matrix, bool bAsAttribute )
{
	KeyValues3* kv = SetKeyValue( id, bAsAttribute );
	if ( kv ) kv->SetMatrix3x4( matrix );
}

CEntityKeyValues::CEntityKeyValues( CKV3Arena* allocator, EntityKVAllocatorType_t allocator_type ) :
	m_pComplexKeys( NULL ),
	m_nRefCount( 0 ),
	m_nQueuedForSpawnCount( 0 ),
	m_bAllowLogging( false ),
	m_eAllocatorType( allocator_type )
{
	if ( allocator )
	{
		m_pAllocator = allocator;

		if ( GameEntitySystem() && m_pAllocator == GameEntitySystem()->GetEntityKeyValuesAllocator() )
			GameEntitySystem()->AddEntityKeyValuesAllocatorRef();

		m_pValues = m_pAllocator->AllocKV();
		m_pAttributes = m_pAllocator->AllocKV();
	}
	else
	{
		if ( !GameEntitySystem() && ( m_eAllocatorType == EKV_ALLOCATOR_ENTSYSTEM_1 || m_eAllocatorType == EKV_ALLOCATOR_ENTSYSTEM_2 ) )
			m_eAllocatorType = EKV_ALLOCATOR_NORMAL;

		m_pAllocator = NULL;
	}
}

CEntityKeyValues::~CEntityKeyValues()
{
	ReleaseAllComplexKeys();

	if ( m_pAllocator )
	{
		if ( m_eAllocatorType != EKV_ALLOCATOR_NORMAL )
		{
			m_pAllocator->FreeKV( m_pValues );
			m_pAllocator->FreeKV( m_pAttributes );

			if ( GameEntitySystem() && m_pAllocator == GameEntitySystem()->GetEntityKeyValuesAllocator() )
				GameEntitySystem()->ReleaseEntityKeyValuesAllocatorRef();
		}
		else
		{
			delete m_pAllocator;
		}
	}
}

void CEntityKeyValues::ValidateAllocator()
{
	if ( !m_pAllocator )
	{
		if ( m_eAllocatorType == EKV_ALLOCATOR_ENTSYSTEM_1 || m_eAllocatorType == EKV_ALLOCATOR_ENTSYSTEM_2 )
		{
			Assert( GameEntitySystem() );
			m_pAllocator = GameEntitySystem()->GetEntityKeyValuesAllocator();
			GameEntitySystem()->AddEntityKeyValuesAllocatorRef();
		}
		else
		{
			Assert( m_eAllocatorType != EKV_ALLOCATOR_EXTERNAL );
			m_pAllocator = new CKV3Arena( true );
		}

		m_pValues = m_pAllocator->AllocKV();
		m_pAttributes = m_pAllocator->AllocKV();
	}
}

void CEntityKeyValues::AddRef()
{
	++m_nRefCount;
}

void CEntityKeyValues::Release()
{
	if ( --m_nRefCount <= 0 )
		delete this;
}

const KeyValues3* CEntityKeyValues::GetKeyValue( const EntityKeyId_t &id, bool* pIsAttribute ) const
{
	if ( !m_pAllocator )
		return NULL;

	const KeyValues3* kv = m_pValues->FindMember( id );

	if ( kv )
	{
		if ( pIsAttribute )
			*pIsAttribute = false;
	}
	else
	{
		kv = m_pAttributes->FindMember( id );

		if ( kv && pIsAttribute )
			*pIsAttribute = true;
	}

	return kv;
}

KeyValues3* CEntityKeyValues::SetKeyValue( const EntityKeyId_t &id, bool bAsAttribute )
{
	if ( m_nQueuedForSpawnCount > 0 )
		return NULL;

	ValidateAllocator();

	bool bIsAttribute;
	KeyValues3* kv = const_cast<KeyValues3*>( GetKeyValue( id, &bIsAttribute ) );

	if ( kv )
	{
		if ( bIsAttribute != bAsAttribute )
		{
			Warning( "Attempted to set existing %s key as %s!\n",
				bIsAttribute ? "attribute" : "non-attribute", bAsAttribute ? "an attribute" : "a non-attribute" );
			return NULL;
		}
	}
	else
	{
		kv = bAsAttribute ? m_pAttributes->FindOrCreateMember( id ) : m_pValues->FindOrCreateMember( id );
	}

	return kv;
}

void CEntityKeyValues::AddConnectionDesc(
	const char* pszOutputName,
	EntityIOTargetType_t eTargetType,
	const char* pszTargetName,
	const char* pszInputName,
	const char* pszOverrideParam,
	float flDelay,
	int32 nTimesToFire,
	const KeyValues3 *kv3value )
{
	if ( m_nQueuedForSpawnCount > 0 )
		return;

	ValidateAllocator();

	EntityIOConnectionDescFat_t* desc = m_connectionDescs.AddToTailGetPtr();

	desc->m_pszOutputName		= m_pAllocator->AllocString( pszOutputName ? pszOutputName : "" );
	desc->m_eTargetType			= eTargetType;
	desc->m_pszTargetName		= m_pAllocator->AllocString( pszTargetName ? pszTargetName : "" );
	desc->m_pszInputName		= m_pAllocator->AllocString( pszInputName ? pszInputName : "" );
	desc->m_pszOverrideParam	= m_pAllocator->AllocString( pszOverrideParam ? pszOverrideParam : "" );
	desc->m_flDelay				= flDelay;
	desc->m_nTimesToFire		= nTimesToFire;

	if ( kv3value )
		desc->m_KV3Value = *kv3value;
}

void CEntityKeyValues::CopyFrom( const CEntityKeyValues* pSrc, bool bRemoveAllKeys, bool bSkipEHandles )
{
	if ( bRemoveAllKeys )
		RemoveAllKeys();

	for ( EntityComplexKeyListElem_t* pListElem = pSrc->m_pComplexKeys; pListElem != NULL; pListElem = pListElem->m_pNext )
	{
		m_pComplexKeys = new EntityComplexKeyListElem_t( pListElem->m_pKey, m_pComplexKeys );
		pListElem->m_pKey->AddRef();
	}

	FOR_EACH_ENTITYKEY( pSrc, iter )
	{
		const char* pAttributeName = NULL;

		if ( pSrc->IsAttribute( iter ) )
		{
			pAttributeName = pSrc->GetAttributeName( iter );
		}
		else if ( bSkipEHandles && pSrc->GetKeyValue( iter )->GetSubType() == KV3_SUBTYPE_EHANDLE )
		{
			continue;
		}

		KeyValues3* kv = SetKeyValue( pSrc->GetEntityKeyId( iter ), pAttributeName != NULL );

		if ( kv )
			*kv = *pSrc->GetKeyValue( iter );
	}

	m_connectionDescs.RemoveAll();
	m_connectionDescs.EnsureCapacity( pSrc->m_connectionDescs.Count() );

	for ( int i = 0; i < pSrc->m_connectionDescs.Count(); i++ )
	{
		const EntityIOConnectionDescFat_t& desc = pSrc->m_connectionDescs[i];
		AddConnectionDesc(
			desc.m_pszOutputName,
			desc.m_eTargetType,
			desc.m_pszTargetName,
			desc.m_pszInputName,
			desc.m_pszOverrideParam,
			desc.m_flDelay,
			desc.m_nTimesToFire,
			&desc.m_KV3Value
		);
	}
}

void CEntityKeyValues::RemoveAllKeys()
{
	if ( m_nQueuedForSpawnCount > 0 )
		return;

	ReleaseAllComplexKeys();

	if ( m_pAllocator )
	{
		if ( m_eAllocatorType != EKV_ALLOCATOR_NORMAL )
		{
			m_pValues->SetToEmptyTable();
			m_pAttributes->SetToEmptyTable();
		}
		else
		{
			m_pAllocator->Clear();
			m_pValues = m_pAllocator->AllocKV();
			m_pAttributes = m_pAllocator->AllocKV();
		}
	}
}

void CEntityKeyValues::ReleaseAllComplexKeys()
{
	EntityComplexKeyListElem_t* pListElem = m_pComplexKeys;

	while ( pListElem != NULL )
	{
		EntityComplexKeyListElem_t* pListElemForRelease = pListElem;
		pListElem = pListElem->m_pNext;
		pListElemForRelease->m_pKey->Release();
		delete pListElemForRelease;
	}

	m_pComplexKeys = NULL;
}

void CEntityKeyValues::SetString( KeyValues3* kv, const char* string )
{
	ValidateAllocator();

	kv->SetStringExternal( m_pAllocator->AllocString( string ) );
}
