#ifndef ISCHEMASYSTEM_TYPESCOPE_H
#define ISCHEMASYSTEM_TYPESCOPE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/platform_base.h"
#include "manipulator.h"

class CSchemaClassInfo;
class CSchemaType;
class CSchemaEnumInfo;
class CSchemaType_Builtin;
class CSchemaType_Ptr;
class CSchemaType_Atomic;
class CSchemaType_Atomic_T;
class CSchemaType_Atomic_CollectionOfT;
class CSchemaType_Atomic_TT;
class CSchemaType_Atomic_I;
class CSchemaType_DeclaredClass;
class CSchemaType_DeclaredEnum;
class CSchemaType_FixedArray;
class CSchemaType_Bitfield;
struct SchemaAtomicTypeInfo_t;

enum SchemaBuiltinType_t
{
	SCHEMA_BUILTIN_TYPE_INVALID = 0,
	SCHEMA_BUILTIN_TYPE_VOID,
	SCHEMA_BUILTIN_TYPE_CHAR,
	SCHEMA_BUILTIN_TYPE_INT8,
	SCHEMA_BUILTIN_TYPE_UINT8,
	SCHEMA_BUILTIN_TYPE_INT16,
	SCHEMA_BUILTIN_TYPE_UINT16,
	SCHEMA_BUILTIN_TYPE_INT32,
	SCHEMA_BUILTIN_TYPE_UINT32,
	SCHEMA_BUILTIN_TYPE_INT64,
	SCHEMA_BUILTIN_TYPE_UINT64,
	SCHEMA_BUILTIN_TYPE_FLOAT32,
	SCHEMA_BUILTIN_TYPE_FLOAT64,
	SCHEMA_BUILTIN_TYPE_BOOL,
	SCHEMA_BUILTIN_TYPE_COUNT,
};

enum SchemaAtomicCategory_t : uint8_t
{
	SCHEMA_ATOMIC_PLAIN = 0,
	SCHEMA_ATOMIC_T,
	SCHEMA_ATOMIC_COLLECTION_OF_T,
	SCHEMA_ATOMIC_TT,
	SCHEMA_ATOMIC_I,
	SCHEMA_ATOMIC_INVALID,
};

enum SchemaTypeScope_t : uint8_t
{
	SCHEMA_GLOBAL_TYPE_SCOPE = 0,
	SCHEMA_MODULE_LOCAL_TYPE_SCOPE,
	SCHEMA_DEFAULT_TYPE_SCOPE,
};

template <class T>
struct SchemaMetaInfoHandle_t
{
	SchemaMetaInfoHandle_t() : m_pObj( NULL ) {}
	SchemaMetaInfoHandle_t( T *obj ) : m_pObj( obj ) {}
	T* Get() const { return m_pObj; }
	bool operator<( const SchemaMetaInfoHandle_t& rhs ) const { return m_pObj < rhs.m_pObj; }

	T* m_pObj;
};

class ISchemaSystemTypeScope
{
public:
	virtual CSchemaClassInfo* InstallSchemaClassBinding( const char* pszModuleName, CSchemaClassInfo* pClassInfo ) = 0;
	virtual CSchemaEnumInfo* InstallSchemaEnumBinding( const char* pszModuleName, CSchemaEnumInfo* pEnumInfo ) = 0;

	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> FindDeclaredClass( const char* pszClassName ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaEnumInfo> FindDeclaredEnum( const char* pszEnumName ) = 0;

	virtual SchemaMetaInfoHandle_t<CSchemaType_Builtin> FindBuiltinTypeByName( const char* pszBuiltinName ) = 0;

	virtual SchemaMetaInfoHandle_t<CSchemaType_Builtin> 		Type_Builtin( SchemaBuiltinType_t eBuiltinType ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Ptr>				Type_Ptr( CSchemaType* pObjectType ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic>			Type_Atomic( const char* pszAtomicName, uint16_t nSize, uint8_t nAlignment ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_T>		Type_Atomic_T( const char* pszAtomicName, uint16_t nSize, uint8_t nAlignment, CSchemaType* pTemplateType ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_CollectionOfT> Type_Atomic_CollectionOfT( const char* pszAtomicName, uint16_t nSize, uint8_t nAlignment, uint16_t nElementSize, CSchemaType* pTemplateType, uint64 nFixedBufferCount, SchemaCollectionManipulatorFn_t manipulator ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_TT>		Type_Atomic_TT( const char* pszAtomicName, uint16_t nSize, uint8_t nAlignment, CSchemaType* pTemplateType, CSchemaType* pTemplateType2 ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_I>		Type_Atomic_I( const char* pszAtomicName, uint16_t nSize, uint8_t nAlignment, int nInteger ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_DeclaredClass>	Type_DeclaredClass( const char* pszClassName ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_DeclaredEnum>	Type_DeclaredEnum( const char* pszEnumName ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_FixedArray>		Type_FixedArray( CSchemaType* pElementType, int nElementCount, uint16_t nElementSize, uint8_t nElementAlignment ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_FixedArray>		Type_FixedArray_Multidimensional( CSchemaType* pElementType, uint16_t nElementSize, uint8_t nElementAlignment, ... ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Bitfield>		Type_Bitfield( int nSize ) = 0;

	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic>			FindType_Atomic( int nAtomicID ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_T>		FindType_Atomic_T( int nAtomicID, CSchemaType* pTemplateType ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_CollectionOfT> FindType_Atomic_CollectionOfT( int nAtomicID, CSchemaType* pTemplateType, SchemaCollectionManipulatorFn_t manipulator ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_TT> 		FindType_Atomic_TT( int nAtomicID, CSchemaType* pTemplateType, CSchemaType* pTemplateType2 ) = 0;
	virtual SchemaMetaInfoHandle_t<CSchemaType_Atomic_I> 		FindType_Atomic_I( int nAtomicID, int nInteger ) = 0;

	virtual CSchemaType_DeclaredClass* FindType_DeclaredClass( const char* pszClassName ) = 0;
	virtual CSchemaType_DeclaredEnum* FindType_DeclaredEnum( const char* pszEnumName ) = 0;

	virtual CSchemaClassInfo* FindRawClassBinding( const char* pszClassName ) = 0;
	virtual CSchemaClassInfo* FindRawClassBinding( uint32_t nClassID ) = 0;
	virtual CSchemaEnumInfo* FindRawEnumBinding( const char* pszEnumName ) = 0;
	virtual CSchemaEnumInfo* FindRawEnumBinding( uint32_t nEnumID ) = 0;

	virtual const char* GetScopeName() = 0;
	virtual bool IsGlobalScope() = 0;

	virtual void MarkClassAsRequiringGlobalPromotion( const CSchemaClassInfo* pClassInfo ) = 0;
	virtual void MarkEnumAsRequiringGlobalPromotion( const CSchemaEnumInfo* pEnumInfo ) = 0;

	virtual void ResolveAtomicInfoThreadsafe( const SchemaAtomicTypeInfo_t** ppAtomicInfo, const char* pszAtomicName, int nAtomicID ) = 0;
	virtual void ResolveEnumInfoThreadsafe( const CSchemaEnumInfo** ppEnumInfo, const char* pszEnumName ) = 0;
	virtual void ResolveClassInfoThreadsafe( const CSchemaClassInfo** ppClassInfo, const char* pszClassName ) = 0;
};

#endif