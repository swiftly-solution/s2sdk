#ifndef ENTITYKEYVALUES_H
#define ENTITYKEYVALUES_H
#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform.h"
#include "public/tier0/keyvalues3.h"
#include "public/tier0/utlleanvector.h"
#include "public/tier0/mempool.h"
#include "entitysystem.h"
#include "variant.h"

#include "public/tier0/memdbgon.h"

class CEntityVariantAllocator
{
public:
	enum { ALWAYS_COPY = 0 };

	static void Free( void *pMemory );

	static void *Allocate( uint nSize );

	static void Activate( CUtlScratchMemoryPool *pMemoryPool, bool bEnable );

private:
	static CUtlScratchMemoryPool *sm_pMemoryPool;
};

typedef CVariantBase<CEntityVariantAllocator> CEntityVariant;

#define FOR_EACH_ENTITYKEY( ekv, iter ) \
	for ( auto iter = ekv->First(); ekv->IsValidIterator( iter ); iter = ekv->Next( iter ) )

typedef CKV3MemberName EntityKeyId_t;

enum EntityKVAllocatorType_t : uint8
{
	EKV_ALLOCATOR_NORMAL = 0,
	EKV_ALLOCATOR_ENTSYSTEM_1 = 1,
	EKV_ALLOCATOR_ENTSYSTEM_2 = 2,
	EKV_ALLOCATOR_EXTERNAL = 3,
};

struct EntityIOConnectionDescFat_t
{ 
	const char* m_pszOutputName;
	EntityIOTargetType_t m_eTargetType;
	const char* m_pszTargetName;
	const char* m_pszInputName;
	const char* m_pszOverrideParam;
	float m_flDelay;
	int32 m_nTimesToFire;

	KeyValues3 m_KV3Value;
};

abstract_class IEntityKeyComplex
{
public:
	IEntityKeyComplex();

	void AddRef();
	void Release();

private:
	virtual void DeleteThis() = 0;

private:
	int32 m_nRefCount;
};

template < class T >
class CEntityKeyComplex : public IEntityKeyComplex
{
public:
	CEntityKeyComplex( const T& obj ) : m_Object( obj ) {}

private:
	virtual void DeleteThis() { delete this; }

public:
	T m_Object;
};

class CEntityKeyValues
{
public:
	CEntityKeyValues( CKV3Arena* allocator = NULL, EntityKVAllocatorType_t allocator_type = EKV_ALLOCATOR_NORMAL );
	~CEntityKeyValues();

	class Iterator_t
	{
		Iterator_t( const KeyValues3* _keys, int _index );
		const KeyValues3* keys;
		int index;
		friend class CEntityKeyValues;
	public:
		bool operator==( const Iterator_t it ) const;
		bool operator!=( const Iterator_t it ) const;
	};
	Iterator_t First() const;
	Iterator_t Next( const Iterator_t &it ) const;
	bool IsValidIterator( const Iterator_t &it ) const;
	const KeyValues3* GetKeyValue( const Iterator_t &it ) const;
	EntityKeyId_t GetEntityKeyId( const Iterator_t &it ) const;
	const char* GetAttributeName( const Iterator_t &it ) const;
	bool IsAttribute( const Iterator_t &it ) const;

	// Indicates that kv's is in the spawn queue and we should not modify it
	bool IsQueuedForSpawn() const;

	bool IsLoggingEnabled() const;
	void EnableLogging( bool bEnable );

	void AddRef();
	void Release();

	void AddConnectionDesc( 
		const char* pszOutputName,	
		EntityIOTargetType_t eTargetType,
		const char* pszTargetName,
		const char* pszInputName,
		const char* pszOverrideParam,
		float flDelay,
		int32 nTimesToFire,
		const KeyValues3 *kv3value = nullptr );

	void RemoveConnectionDesc( int nDesc );

	EntityIOConnectionDescFat_t* GetConnectionDesc( int nDesc );
	const EntityIOConnectionDescFat_t* GetConnectionDesc( int nDesc ) const;
	int GetNumConnectionDescs() const;

	void CopyFrom( const CEntityKeyValues* pSrc, bool bRemoveAllKeys = false, bool bSkipEHandles = false );
	void RemoveKeyValue( const EntityKeyId_t &id );
	void RemoveAllKeys();

	bool IsEmpty() const;

	bool HasValue( const EntityKeyId_t &id ) const;
	bool HasAttribute( const EntityKeyId_t &id ) const;

	const KeyValues3* GetKeyValue( const EntityKeyId_t &id, bool* pIsAttribute = NULL ) const;

	//
	// GetKeyValue helpers
	// 
	bool			GetBool( const EntityKeyId_t &id, bool defaultValue = false ) const;
	int				GetInt( const EntityKeyId_t &id, int defaultValue = 0 ) const;
	uint			GetUint( const EntityKeyId_t &id, uint defaultValue = 0 ) const;
	int64			GetInt64( const EntityKeyId_t &id, int64 defaultValue = 0 ) const;
	uint64			GetUint64( const EntityKeyId_t &id, uint64 defaultValue = 0 ) const;
	float			GetFloat( const EntityKeyId_t &id, float defaultValue = 0.0f ) const;
	double			GetDouble( const EntityKeyId_t &id, double defaultValue = 0.0 ) const;
	const char*		GetString( const EntityKeyId_t &id, const char *defaultValue = "" ) const;
	void*			GetPtr( const EntityKeyId_t &id, void *defaultValue = ( void* )0 ) const;
	CUtlStringToken	GetStringToken( const EntityKeyId_t &id, CUtlStringToken defaultValue = CUtlStringToken() ) const;
	CEntityHandle	GetEHandle( const EntityKeyId_t &id, WorldGroupId_t worldGroupId = WorldGroupId_t(), CEntityHandle defaultValue = CEntityHandle() ) const;
	Color			GetColor( const EntityKeyId_t &id, const Color &defaultValue = Color( 0, 0, 0, 255 ) ) const;
	Vector			GetVector( const EntityKeyId_t &id, const Vector &defaultValue = Vector( 0.0f, 0.0f, 0.0f ) ) const;
	Vector2D		GetVector2D( const EntityKeyId_t &id, const Vector2D &defaultValue = Vector2D( 0.0f, 0.0f ) ) const;
	Vector4D		GetVector4D( const EntityKeyId_t &id, const Vector4D &defaultValue = Vector4D( 0.0f, 0.0f, 0.0f, 0.0f ) ) const;
	Quaternion		GetQuaternion( const EntityKeyId_t &id, const Quaternion &defaultValue = Quaternion( 0.0f, 0.0f, 0.0f, 0.0f ) ) const;
	QAngle			GetQAngle( const EntityKeyId_t &id, const QAngle &defaultValue = QAngle( 0.0f, 0.0f, 0.0f ) ) const;
	matrix3x4_t		GetMatrix3x4( const EntityKeyId_t &id, const matrix3x4_t &defaultValue = matrix3x4_t( Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ), Vector( 0.0f, 0.0f, 0.0f ) ) ) const;

	void SetBool( const EntityKeyId_t &id, bool value, bool bAsAttribute = false );
	void SetInt( const EntityKeyId_t &id, int value, bool bAsAttribute = false );
	void SetUint( const EntityKeyId_t &id, uint value, bool bAsAttribute = false );
	void SetInt64( const EntityKeyId_t &id, int64 value, bool bAsAttribute = false );
	void SetUint64( const EntityKeyId_t &id, uint64 value, bool bAsAttribute = false );
	void SetFloat( const EntityKeyId_t &id, float value, bool bAsAttribute = false );
	void SetDouble( const EntityKeyId_t &id, double value, bool bAsAttribute = false );
	void SetString( const EntityKeyId_t &id, const char* string, bool bAsAttribute = false );
	void SetPtr( const EntityKeyId_t &id, void* ptr, bool bAsAttribute = false );
	void SetStringToken( const EntityKeyId_t &id, CUtlStringToken token, bool bAsAttribute = false );
	void SetEHandle( const EntityKeyId_t &id, CEntityHandle ehandle, bool bAsAttribute = false );
	void SetColor( const EntityKeyId_t &id, const Color &color, bool bAsAttribute = false );
	void SetVector( const EntityKeyId_t &id, const Vector &vec, bool bAsAttribute = false );
	void SetVector2D( const EntityKeyId_t &id, const Vector2D &vec2d, bool bAsAttribute = false );
	void SetVector4D( const EntityKeyId_t &id, const Vector4D &vec4d, bool bAsAttribute = false );
	void SetQuaternion( const EntityKeyId_t &id, const Quaternion &quat, bool bAsAttribute = false );
	void SetQAngle( const EntityKeyId_t &id, const QAngle &ang, bool bAsAttribute = false );
	void SetMatrix3x4( const EntityKeyId_t &id, const matrix3x4_t &matrix, bool bAsAttribute = false );	

	CEntityKeyValues& operator=( const CEntityKeyValues& src );

private:
	CEntityKeyValues( const CEntityKeyValues& other );

	// Use public setters for all available types instead
	KeyValues3* SetKeyValue( const EntityKeyId_t &id, bool bAsAttribute = false );
	void SetString( KeyValues3* kv, const char* string );

	void ReleaseAllComplexKeys();
	void ValidateAllocator();

private:
	struct EntityComplexKeyListElem_t 
	{
		EntityComplexKeyListElem_t( IEntityKeyComplex* pKey, EntityComplexKeyListElem_t* pNext );

		IEntityKeyComplex* m_pKey;
		EntityComplexKeyListElem_t* m_pNext;
	};

	CKV3Arena* m_pAllocator;
	EntityComplexKeyListElem_t* m_pComplexKeys;
	KeyValues3* m_pValues;
	KeyValues3* m_pAttributes;
	int16 m_nRefCount;
	int16 m_nQueuedForSpawnCount;
	bool m_bAllowLogging;
	EntityKVAllocatorType_t m_eAllocatorType;
	CUtlLeanVector<EntityIOConnectionDescFat_t> m_connectionDescs;
};











































#include "public/tier0/memdbgoff.h"

#endif // ENTITYKEYVALUES_H
