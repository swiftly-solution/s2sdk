#ifndef RESOURCEHANDLE_H
#define RESOURCEHANDLE_H

#ifdef COMPILER_MSVC
#pragma once
#endif

#include "public/resourcefile/resourcetype.h"
#include "resourcesystemtypes.h"

struct ResourceSerialHandle_t
{
	ResourceSerialHandle_t();
	ResourceSerialHandle_t( uint64 nValue );

	uint64 m_Value;
};

abstract_class IResourceHandleUtils
{
public:
	virtual ResourceHandle_t FindOrRegisterResourceByName( const CResourceName &name ) = 0;
	virtual ResourceHandle_t FindResourceById( ResourceId_t id, ResourceType_t nType ) = 0;
	virtual void DeleteResource( ResourceHandle_t hResource ) = 0;
	virtual void ResourceReferenceLeakTracking_AddRef( ResourceHandle_t hResource, ResourceLeakTrackingGroup_t eGroup, uintp nContext ) = 0;
	virtual void ResourceReferenceLeakTracking_Release( ResourceHandle_t hResource, ResourceLeakTrackingGroup_t eGroup, uintp nContext ) = 0;
	virtual void ResourceReferenceLeakTracking_ReportReferences( ResourceHandle_t hResource ) = 0;
	virtual ResourceStatus_t GetResourceStatus( ResourceHandle_t hResource ) = 0;
	virtual ResourceType_t GetResourceHandleType( ResourceHandle_t hResource ) = 0;
	// AMNOTE: Runs the handle allocator self-test
	virtual void unk101() = 0;
	// AMNOTE: Returns hResource as a serial handle
	virtual ResourceSerialHandle_t unk102( ResourceHandle_t hResource ) = 0;
	virtual ResourceHandle_t HandleUtils_Deref( ResourceSerialHandle_t hSerial ) = 0;
};

#endif // RESOURCEHANDLE_H
