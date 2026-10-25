#ifndef ISCHEMASYSTEM_H
#define ISCHEMASYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "typescope.h"
#include "public/appframework/iappsystem.h"
#include "public/tier0/bufferstring.h"
#include "public/tier0/utlvector.h"

#define SCHEMASYSTEM_INTERFACE_VERSION "SchemaSystem_001"

class CKV3Arena;

enum SchemaHierarchyIteration_t
{
	SCHEMA_ITER_NONE = 0,
	SCHEMA_ITER_SINGLE_PARENT,
	SCHEMA_ITER_MULTI_PARENT
};

class ISchemaSystem : public IAppSystem
{
public:
	virtual ISchemaSystemTypeScope* GlobalTypeScope() = 0;
	virtual ISchemaSystemTypeScope* FindOrCreateTypeScopeForModule( const char* pszModuleName, const char** ppszBindingName = NULL ) = 0;
	virtual ISchemaSystemTypeScope* FindTypeScopeForModule( const char* pszModuleName, const char** ppszBindingName = NULL ) = 0;
	virtual ISchemaSystemTypeScope* GetTypeScopeForBinding( SchemaTypeScope_t eTypeScope, const char* pszModuleName, const char** ppszBindingName = NULL ) = 0;
	
	virtual bool DefaultTypeScopeIsLocal() = 0;
	
	virtual SchemaMetaInfoHandle_t<CSchemaClassInfo> FindClassByScopedName( const char* pszScopedName ) = 0;
	virtual void ScopedNameForClass( const CSchemaClassInfo* pClassInfo, CBufferString& scopedName ) = 0;
	
	virtual SchemaMetaInfoHandle_t<CSchemaEnumInfo> FindEnumByScopedName( const char* pszScopedName ) = 0;
	virtual void ScopedNameForEnum( const CSchemaEnumInfo* pEnumInfo, CBufferString& scopedName ) = 0;
	
	virtual void FindDescendentsOfClass( const CSchemaClassInfo* pClassInfo, int, CUtlVector<const CSchemaClassInfo*> *descendents, bool ) = 0;
	virtual void LoadSchemaDataForModules( const char** ppszModules, int nModules ) = 0;
	
	virtual const char* GetClassModuleName( const CSchemaClassInfo* pClassInfo ) = 0;
	virtual const char* GetClassProjectName( const CSchemaClassInfo* pClassInfo ) = 0;
	
	virtual const char* GetEnumModuleName( const CSchemaEnumInfo* pEnumInfo ) = 0;
	virtual const char* GetEnumProjectName( const CSchemaEnumInfo* pEnumInfo ) = 0;
	
	virtual bool SchemaSystemIsReady() = 0;
	virtual void VerifySchemaBindingConsistency( bool ) = 0;
	virtual void CompleteModuleRegistration( const char* pszModuleName ) = 0;
	virtual void RegisterAtomicType( const SchemaAtomicTypeInfo_t* pAtomicType ) = 0;
	
	virtual void PrintSchemaStats( const char* pszOptions ) = 0;
	virtual void PrintSchemaMetaStats( const char* pszOptions ) = 0;
	
	virtual void RegisterAtomics( const char* pszModuleName, const char* pszProjectName, CSchemaType** ppSchemaTypes, int, SchemaAtomicTypeInfo_t** ppAtomicTypes ) = 0;
	virtual void RegisterEnums( const char* pszModuleName, const char* pszProjectName, CSchemaType** ppSchemaTypes, int, CSchemaEnumInfo** ppEnumInfos ) = 0;
	virtual bool RegisterClasses( const char* pszModuleName, const char* pszProjectName, CSchemaType** ppSchemaTypes, int, CSchemaClassInfo** ppClassInfos, CBufferString* pErrorStr ) = 0;
	virtual void ValidateClasses( CSchemaClassInfo** ppClassInfos ) = 0;
	
	virtual bool ConvertOldIntrospectedResourceDataToKV3( void*, void*, void*, CKV3Arena*, const char* ) = 0;
	virtual void FindClassesByMeta( const char* pszMetaName, SchemaHierarchyIteration_t iter_type, CUtlVector<const CSchemaClassInfo*> *classes ) = 0;
	
	virtual SchemaMetaInfoHandle_t<CSchemaType_Builtin> GetSchemaBuiltinType( SchemaBuiltinType_t eBuiltinType ) = 0;
	
	virtual ~ISchemaSystem() = 0;
};

#endif