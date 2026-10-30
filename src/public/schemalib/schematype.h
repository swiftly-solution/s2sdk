#ifndef SCHEMATYPE_H
#define SCHEMATYPE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/generichash.h"
#include "public/tier0/loggingtypes.h"
#include "public/tier0/platform.h"
#include "public/tier0/utlstring.h"
#include "schemametadata_data.h"
#include "public/schemasystem/typescope.h"

class CSchemaSystemTypeScope;
class CSchemaClassInfo;
class CSchemaEnumInfo;
class CBufferString;

struct SchemaAtomicTypeInfo_t
{
	const char* m_pszName;
	const char* m_pszTokenName;
	
	int m_nAtomicID;
	
	int m_nStaticMetadataCount;
	SchemaMetadataEntryData_t* m_pStaticMetadata;
};

template <typename T>
struct SchemaTypeMap {};

#define SCHEMATYPE_ENTRY( type, type_cat, atomic_cat ) template <> struct SchemaTypeMap<type> \
{ \
	static const SchemaTypeCategory_t type_category = type_cat; \
	static const SchemaAtomicCategory_t atomic_category = atomic_cat; \
	static inline bool Match( CSchemaType *ptr ) { return ptr->m_eTypeCategory == type_category && ptr->m_eAtomicCategory == atomic_category; } \
};

enum SchemaTypeCategory_t : uint8_t
{
	SCHEMA_TYPE_BUILTIN = 0,
	SCHEMA_TYPE_POINTER,
	SCHEMA_TYPE_BITFIELD,
	SCHEMA_TYPE_FIXED_ARRAY,
	SCHEMA_TYPE_ATOMIC,
	SCHEMA_TYPE_DECLARED_CLASS,
	SCHEMA_TYPE_DECLARED_ENUM,
	SCHEMA_TYPE_INVALID,
};

uint32_t CSchemaType_Hash( const char *pString, int len );

class CSchemaType
{
public:
	virtual bool IsValid();
	virtual const char *ToString( CBufferString &buff, bool bDontClearBuff );
	virtual void SpewDescription( LoggingChannelID_t channelID, const char *pszName );
	virtual bool GetSizeAndAlignment( int &nSize, uint8_t &nAlignment );
	virtual bool CanReinterpretAs( const CSchemaType *pType );
	virtual SchemaMetaInfoHandle_t<CSchemaType> GetInnerType();
	virtual SchemaMetaInfoHandle_t<CSchemaType> GetInnermostType();

	// Returns true if pType is this object
	virtual bool IsA( const CSchemaType *pType );
	virtual CSchemaType *InternalMatchInnerAs( SchemaTypeCategory_t eTypeCategory, SchemaAtomicCategory_t eAtomicCategory );

	// Returns true if this type matches pType type traits
	virtual bool IsTypeByType( const CSchemaType *pType );
	// Returns true if this type name matches szName
	virtual bool IsTypeByName( const char *szName );

	// Returns true if this type is builtin and matches eType
	virtual bool IsBuiltinType( SchemaBuiltinType_t eType );
	// Returns true if this type is atomic and matches nAtomicID
	virtual bool IsAtomicType( int nAtomicID );
	// Returns true if this type is declared class and matches szClassName
	virtual bool IsDeclaredClass( const char *szClassName );
	// Returns true if this type is declared enum and matches szEnumName
	virtual bool IsDeclaredEnum( const char *szEnumName );

	virtual bool DependsOnlyOnUnresolvedOrGlobalTypes( ISchemaSystemTypeScope *pTypeScope );

	virtual ~CSchemaType() = 0;

	template <typename T>
	bool IsA() { return SchemaTypeMap<T>::Match( this ); }

	template <typename T>
	T *ReinterpretAs() { return (IsA<T>()) ? (T *)this : nullptr; }

public:
	CUtlString m_sTypeName;
	CSchemaSystemTypeScope* m_pTypeScope;
	SchemaTypeCategory_t m_eTypeCategory;
	SchemaAtomicCategory_t m_eAtomicCategory;
};

class CSchemaType_Builtin : public CSchemaType
{
public:
	SchemaBuiltinType_t m_eBuiltinType;
	uint8_t m_nSize;
};

class CSchemaType_Ptr : public CSchemaType
{
public:
	CSchemaType* m_pObjectType;
};

class CSchemaType_Atomic : public CSchemaType
{
public:
	SchemaAtomicTypeInfo_t* m_pAtomicInfo;
	int m_nAtomicID;
	uint16_t m_nSize;
	uint8_t m_nAlignment;
};

class CSchemaType_Atomic_T : public CSchemaType_Atomic
{
public:
	CSchemaType* m_pTemplateType;
};

class CSchemaType_Atomic_CollectionOfT : public CSchemaType_Atomic_T
{
public:
	SchemaCollectionManipulatorFn_t m_pfnManipulator;
	uint16_t m_nElementSize;
	uint64 m_nFixedBufferCount;
};

class CSchemaType_Atomic_TT : public CSchemaType_Atomic_T
{
public:
	CSchemaType* m_pTemplateType2;
};

class CSchemaType_Atomic_I : public CSchemaType_Atomic
{
public:
	int m_nInteger;
};

class CSchemaType_DeclaredClass : public CSchemaType
{
public:
	CSchemaClassInfo* m_pClassInfo;
	bool m_bGlobalPromotionRequired;
};

class CSchemaType_DeclaredEnum : public CSchemaType
{
public:
	CSchemaEnumInfo* m_pEnumInfo;
	bool m_bGlobalPromotionRequired;
};

class CSchemaType_FixedArray : public CSchemaType
{
public:
	int m_nElementCount;
	uint16_t m_nElementSize;
	uint8_t m_nElementAlignment;
	CSchemaType* m_pElementType;
};

class CSchemaType_Bitfield : public CSchemaType
{
public:
	int m_nBitfieldCount;
};

SCHEMATYPE_ENTRY( CSchemaType_Builtin, SCHEMA_TYPE_BUILTIN, SCHEMA_ATOMIC_INVALID );
SCHEMATYPE_ENTRY( CSchemaType_Ptr, SCHEMA_TYPE_POINTER, SCHEMA_ATOMIC_INVALID );
SCHEMATYPE_ENTRY( CSchemaType_Atomic, SCHEMA_TYPE_ATOMIC, SCHEMA_ATOMIC_PLAIN );
SCHEMATYPE_ENTRY( CSchemaType_Atomic_T, SCHEMA_TYPE_ATOMIC, SCHEMA_ATOMIC_T );
SCHEMATYPE_ENTRY( CSchemaType_Atomic_CollectionOfT, SCHEMA_TYPE_ATOMIC, SCHEMA_ATOMIC_COLLECTION_OF_T );
SCHEMATYPE_ENTRY( CSchemaType_Atomic_TT, SCHEMA_TYPE_ATOMIC, SCHEMA_ATOMIC_TT );
SCHEMATYPE_ENTRY( CSchemaType_Atomic_I, SCHEMA_TYPE_ATOMIC, SCHEMA_ATOMIC_I );
SCHEMATYPE_ENTRY( CSchemaType_DeclaredClass, SCHEMA_TYPE_DECLARED_CLASS, SCHEMA_ATOMIC_INVALID );
SCHEMATYPE_ENTRY( CSchemaType_DeclaredEnum, SCHEMA_TYPE_DECLARED_ENUM, SCHEMA_ATOMIC_INVALID );
SCHEMATYPE_ENTRY( CSchemaType_FixedArray, SCHEMA_TYPE_FIXED_ARRAY, SCHEMA_ATOMIC_INVALID );
SCHEMATYPE_ENTRY( CSchemaType_Bitfield, SCHEMA_TYPE_BITFIELD, SCHEMA_ATOMIC_INVALID );

#endif