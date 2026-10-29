#ifndef IAPPSYSTEM_H
#define IAPPSYSTEM_H

#ifdef _WIN32
#pragma once
#endif

#include "public/interfaces/interfaces.h"

struct AppSystemInfo_t
{
	const char *m_pModuleName;
	const char *m_pInterfaceName;
};

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL,
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,
	APP_SYSTEM_TIER4,
	APP_SYSTEM_TIER5,

	APP_SYSTEM_TIER_OTHER,
};

enum BuildType_t
{
	kBuildTypeRelease = 2
};

class IAppSystem
{
public:
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
	virtual void PreShutdown() = 0;
	virtual const AppSystemInfo_t* GetDependencies() = 0;
	virtual AppSystemTier_t GetTier() = 0;
	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName ) = 0;
	virtual bool IsSingleton() = 0;
	virtual BuildType_t	GetBuildType() = 0;
};

template< class IInterface >
class CBaseAppSystem : public IInterface
{
public:
	virtual bool Connect( CreateInterfaceFn factory ) { return true; }
	virtual void Disconnect() {}
	virtual void *QueryInterface( const char *pInterfaceName ) { return NULL; }
	virtual InitReturnVal_t Init() { return INIT_OK; }
	virtual void Shutdown() {}
	virtual void PreShutdown() {}
	virtual const AppSystemInfo_t* GetDependencies() { return NULL; }
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER_OTHER; }
	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName )
	{
		ReconnectInterface( factory, pInterfaceName );
	}
	virtual bool IsSingleton() { return true; }
	virtual BuildType_t GetBuildType() { return kBuildTypeRelease; }
};

template< class IInterface >
class CTier0AppSystem : public CBaseAppSystem< IInterface >
{
public:
	virtual AppSystemTier_t GetTier() { return APP_SYSTEM_TIER0; }
};

#endif