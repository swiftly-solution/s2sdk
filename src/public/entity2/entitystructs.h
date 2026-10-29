#ifndef ENTITYSTRUCTS_H
#define ENTITYSTRUCTS_H

#if _WIN32
#pragma once
#endif

class CEntityKeyValues;
class IEntityPrecacheConfiguration;
class IEntityResourceManifest;

struct CEntityPrecacheContext
{
	const CEntityKeyValues* m_pKeyValues;

	// AMNOTE: Opaque to the game, the engine's resource manifests pass nullptr
	IEntityPrecacheConfiguration* m_pConfig;

	IEntityResourceManifest* m_pManifest;
};

#endif