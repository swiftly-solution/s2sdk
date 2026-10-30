#ifndef RESOURCETYPE_H
#define RESOURCETYPE_H

#ifdef _WIN32
#pragma once
#endif

#include "public/tier0/bufferstring.h"
#include "public/tier0/utlsymbollarge.h"
#include "public/tier0/threadtools.h"

enum ResourceBindingFlags_t
{
	RESOURCE_BINDING_LOADED = 0x1,
	RESOURCE_BINDING_ERROR = 0x2,
	RESOURCE_BINDING_UNLOADABLE = 0x4,
	RESOURCE_BINDING_PROCEDURAL = 0x8,
	RESOURCE_BINDING_TRACKLEAKS = 0x20,
	RESOURCE_BINDING_IS_ERROR_BINDING_FOR_TYPE = 0x40,
	RESOURCE_BINDING_HAS_EVER_BEEN_LOADED = 0x80,
	RESOURCE_BINDING_ANONYMOUS = 0x100,

	RESOURCE_BINDING_FIRST_UNUSED_FLAG = 0x200
};

struct ResourceId_t
{
	ResourceId_t();
	ResourceId_t( uint64 nValue );

	uint64 m_Value;
};

struct ResourceNameInfo_t
{
	CUtlSymbolLarge m_ResourceNameSymbol;
};

typedef const ResourceNameInfo_t *ResourceNameHandle_t;
typedef uint16_t LoadingResourceIndex_t;
typedef uint16_t StreamingResourceDataIndex_t;
typedef char ResourceTypeIndex_t;
typedef uint32_t ExtRefIndex_t;

struct ResourceBindingBase_t
{
	void* m_pData;
	ResourceNameHandle_t m_Name;
	ResourceId_t m_ResourceId;
	uint16_t m_nFlags;
	StreamingResourceDataIndex_t m_nStreamingResource;
	ResourceTypeIndex_t m_nResourceType;
	uint8_t m_nReloadCounter;
	LoadingResourceIndex_t m_nLoadingResource;
	CInterlockedInt m_nRefCount;
	ExtRefIndex_t m_nExtRefHandle;
};

typedef const ResourceBindingBase_t* ResourceHandle_t;
typedef void* HGameResourceManifest;
typedef uint64 ResourceType_t;

class CResourceName
{
public:
	CResourceName();
	CResourceName( const char *pszName );
	CResourceName( const char *pszName, ResourceType_t nType );
	CResourceName( const CResourceName &other );

	CResourceName &operator=( const CResourceName &other );

	void CopyFrom( const CResourceName &other );

	bool Init( const char *pszName );
	bool InitAndForceType( const char *pszName, ResourceType_t nType );

	void Clear();

	bool IsEmpty() const;
	const char *GetName() const;
	ResourceId_t GetId() const;
	ResourceType_t GetType() const;

protected:
	CPathBufferString m_Name;
	ResourceId_t m_Id;
	ResourceType_t m_Type;
};

bool FixupResourceName( CBufferString &name );
bool FixupResourceName( ResourceType_t nType, CBufferString &name );
ResourceType_t DeduceResourceTypeFromResourceName( const char *pszName );

#endif