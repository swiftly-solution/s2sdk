#include "public/resourcesystem/iresourcesystem.h"

IAsyncProcessedDataResult::~IAsyncProcessedDataResult()
{}

ResourceManifestCreationInfo_t::ResourceManifestCreationInfo_t()
:
	m_nCount( 0 ), m_ppResourceFiles( nullptr ), m_nType( RESOURCE_MANIFEST_TYPE_DEFAULT ), m_nLoadBehavior( RESOURCE_MANIFEST_LOAD_DEFAULT ),
	m_pDebugName( nullptr ), m_nPriority( RESOURCE_MANIFEST_LOAD_PRIORITY_DEFAULT ), m_pfnCallback( nullptr ), m_pContext( nullptr )
{ }

ResourceManifestCreationInfo_t::ResourceManifestCreationInfo_t( int nCount, const char *const *ppResourceFiles, ResourceManifestType_t nType, ResourceManifestLoadBehavior_t nLoadBehavior,
	const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
:
	m_nCount( nCount ), m_ppResourceFiles( ppResourceFiles ), m_nType( nType ), m_nLoadBehavior( nLoadBehavior ),
	m_pDebugName( pDebugName ), m_nPriority( nPriority ), m_pfnCallback( nullptr ), m_pContext( nullptr )
{ }

ResourceManifestCreationInfo_t::ResourceManifestCreationInfo_t( int nCount, const char *const *ppResourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior,
	const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
:
	ResourceManifestCreationInfo_t( nCount, ppResourceFiles, RESOURCE_MANIFEST_TYPE_DEFAULT, nLoadBehavior, pDebugName, nPriority )
{ }

ResourceManifestCreationInfo_t::ResourceManifestCreationInfo_t( const char *const &pszResourceFile, ResourceManifestLoadBehavior_t nLoadBehavior,
	const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
:
	ResourceManifestCreationInfo_t( 1, &pszResourceFile, nLoadBehavior, pDebugName, nPriority )
{ }

ResourceManifestCreationInfo_t::ResourceManifestCreationInfo_t( std::initializer_list<const char *> resourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior,
	const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
:
	ResourceManifestCreationInfo_t( (int)resourceFiles.size(), resourceFiles.begin(), nLoadBehavior, pDebugName, nPriority )
{ }

HResourceManifest IResourceSystem::CreateResourceManifest( int nCount, const char *const *ppResourceFiles, ResourceManifestType_t nType, ResourceManifestLoadBehavior_t nLoadBehavior, const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
{
	return CreateResourceManifest( ResourceManifestCreationInfo_t( nCount, ppResourceFiles, nType, nLoadBehavior, pDebugName, nPriority ) );
}

HResourceManifest IResourceSystem::CreateResourceManifest( int nCount, const char *const *ppResourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior, const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
{
	return CreateResourceManifest( ResourceManifestCreationInfo_t( nCount, ppResourceFiles, nLoadBehavior, pDebugName, nPriority ) );
}

HResourceManifest IResourceSystem::CreateResourceManifest( std::initializer_list<const char *> resourceFiles, ResourceManifestLoadBehavior_t nLoadBehavior,
	const char *pDebugName, ResourceManifestLoadPriority_t nPriority )
{
	return CreateResourceManifest( ResourceManifestCreationInfo_t( resourceFiles, nLoadBehavior, pDebugName, nPriority ) );
}
