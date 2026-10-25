#ifndef SCHEMASYSTEM_ATOMICTYPES_H
#define SCHEMASYSTEM_ATOMICTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "public/schemalib/schematype.h"

#include "public/tier0/threadtools.h"
#include "public/tier0/utlmap.h"
#include "public/tier0/utlvector.h"
#include "public/tier0/utlstringmap.h"
#include "public/tier0/utltshash.h"

#include "manipulator.h"
#include "schemasystem.h"

struct ResourceManifestDesc_t;

template <class K, class V>
class CSchemaPtrMap
{
public:
	CUtlOrderedMap<K, V, CDefLess<K>, unsigned short> m_Map;
	CThreadFastMutex m_Mutex;
};

struct AtomicTypeInfo_T_t
{
	bool operator<( const AtomicTypeInfo_T_t& rhs ) const 
	{ 
		if ( m_nAtomicID != rhs.m_nAtomicID )
			return m_nAtomicID < rhs.m_nAtomicID;
		
		if ( m_pTemplateType != rhs.m_pTemplateType )
			return m_pTemplateType < rhs.m_pTemplateType;
		
		return (void *)m_pfnManipulator < (void *)rhs.m_pfnManipulator;
	}
	
	int m_nAtomicID;
	CSchemaType* m_pTemplateType;
	SchemaCollectionManipulatorFn_t m_pfnManipulator;
};

struct AtomicTypeInfo_CollectionOfT_t
{
	bool operator<( const AtomicTypeInfo_CollectionOfT_t &rhs ) const
	{
		if(m_nAtomicID != rhs.m_nAtomicID)
			return m_nAtomicID < rhs.m_nAtomicID;

		if(m_pTemplateType != rhs.m_pTemplateType)
			return m_pTemplateType < rhs.m_pTemplateType;

		if(m_nFixedBufferCount != rhs.m_nFixedBufferCount)
			return m_nFixedBufferCount < rhs.m_nFixedBufferCount;

		return (void *)m_pfnManipulator < (void *)rhs.m_pfnManipulator;
	}

	int m_nAtomicID;
	CSchemaType *m_pTemplateType;
	uint64_t m_nFixedBufferCount;
	SchemaCollectionManipulatorFn_t m_pfnManipulator;
};

struct AtomicTypeInfo_TT_t
{
	bool operator<( const AtomicTypeInfo_TT_t& rhs ) const 
	{ 
		if ( m_nAtomicID != rhs.m_nAtomicID )
			return m_nAtomicID < rhs.m_nAtomicID;
		
		if ( m_pTemplateType != rhs.m_pTemplateType )
			return m_pTemplateType < rhs.m_pTemplateType;
		
		return m_pTemplateType2 < rhs.m_pTemplateType2;
	}
	
	int m_nAtomicID;
	CSchemaType* m_pTemplateType;
	CSchemaType* m_pTemplateType2;
};

struct AtomicTypeInfo_I_t
{
	bool operator<( const AtomicTypeInfo_I_t& rhs ) const 
	{ 
		if ( m_nAtomicID != rhs.m_nAtomicID )
			return m_nAtomicID < rhs.m_nAtomicID;

		return m_nInteger < rhs.m_nInteger;
	}
	
	int m_nAtomicID;
	int m_nInteger;
};

struct TypeAndCountInfo_t
{
	bool operator<( const TypeAndCountInfo_t& rhs ) const 
	{ 
		if ( m_nElementCount != rhs.m_nElementCount )
			return m_nElementCount < rhs.m_nElementCount;
		
		return m_pElementType < rhs.m_pElementType;
	}
	
	int m_nElementCount;
	CSchemaType* m_pElementType;
};

class CSchemaSystemTypeScope : public ISchemaSystemTypeScope
{
public:
	char m_szScopeName[256];
	
	CSchemaSystemTypeScope* m_pGlobalTypeScope;
	
	bool m_bBuiltinTypesInitialized;
	CSchemaType_Builtin m_BuiltinTypes[SCHEMA_BUILTIN_TYPE_COUNT];
	
	CSchemaPtrMap<SchemaMetaInfoHandle_t<CSchemaType>, CSchemaType_Ptr*> m_Pointers;
	CSchemaPtrMap<int, CSchemaType_Atomic*>							m_Atomics;
	CSchemaPtrMap<AtomicTypeInfo_T_t, CSchemaType_Atomic_T*>		m_AtomicsT;
	CSchemaPtrMap<AtomicTypeInfo_CollectionOfT_t, CSchemaType_Atomic_CollectionOfT*> m_AtomicsCollectionOfT;
	CSchemaPtrMap<AtomicTypeInfo_TT_t, CSchemaType_Atomic_TT*>		m_AtomicsTT;
	CSchemaPtrMap<AtomicTypeInfo_I_t, CSchemaType_Atomic_I*>		m_AtomicsI;
	CSchemaPtrMap<uint32_t, CSchemaType_DeclaredClass*>					m_DeclaredClasses;
	CSchemaPtrMap<uint32_t, CSchemaType_DeclaredEnum*> 					m_DeclaredEnums;
	CSchemaPtrMap<int, SchemaMetaInfoHandle_t<SchemaAtomicTypeInfo_t>> m_AtomicInfos;
	CSchemaPtrMap<TypeAndCountInfo_t, CSchemaType_FixedArray*> 		m_FixedArrays;
	CSchemaPtrMap<int, CSchemaType_Bitfield*> 						m_Bitfields;
	
	CUtlTSHash<CSchemaClassInfo*, 256, uint32_t> m_ClassBindings;
	CUtlTSHash<CSchemaEnumInfo*, 256, uint32_t> m_EnumBindings;
};

class CSchemaSystem : public ISchemaSystem
{
	struct DetectedSchemaMismatch_t
	{
		CUtlString m_sBinding;
		CUtlString m_sMismatch;
		CUtlString m_sModule1;
		CUtlString m_sModule2;
	};
	
	struct BindingsAddressRangeForModule_t
	{
		uintptr_t m_minAddr;
		uintptr_t m_maxAddr;
	};

public:
	CUtlVector<ResourceManifestDesc_t*> m_ResourceManifestDescs;
	int m_nNumConnections;

private:
	// AMNOTE: isn't used anywhere at construction/destruction, most likely padding
	int m_unk001;

public:
	CThreadFastMutex m_Mutex;

	CConCommandMemberAccessor<CSchemaSystem> m_SchemaListBindings;
	CConCommandMemberAccessor<CSchemaSystem> m_SchemaAllListBindings;
	CConCommandMemberAccessor<CSchemaSystem> m_SchemaDumpBinding;
	CConCommandMemberAccessor<CSchemaSystem> m_SchemaDetailedClassLayout;
	CConCommandMemberAccessor<CSchemaSystem> m_SchemaStats;
	CConCommandMemberAccessor<CSchemaSystem> m_SchemaMetaStats;
	
	CUtlVector<void*> m_LoadedModules;
	CUtlVector<DetectedSchemaMismatch_t> m_DetectedSchemaMismatches;
	
	bool m_bDefaultTypeScopeIsLocal;
	bool m_bSchemaSystemIsReady;
	
	CUtlStringMap<CSchemaSystemTypeScope*> m_TypeScopes;
	CUtlStringMap<BindingsAddressRangeForModule_t> m_BindingsAddressRanges;
	
	int m_nRegistrations;
	int m_nIgnored;
	int m_nRedundant;
	size_t m_nIgnoredBytes;
};

#endif