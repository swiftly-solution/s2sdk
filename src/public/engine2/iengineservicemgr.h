//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef IENGINESERVICEMGR_H
#define IENGINESERVICEMGR_H
#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "hoststate.h"
#include "public/inputsystem/inputenums.h"
#include "public/tier0/keyvalues.h"
#include "public/tier0/eventdispatcher.h"
#include "public/tier0/convar.h"
#include "public/tier0/utlstring.h"
#include "public/localize/ilocalize.h"
#include "tier4.h"
#include "iprerequisite_registry.h"
#include "public/rendersystem/rendersystemtypes.h"
#include "engineevents.h"

class IAddonListChangeNotify;
struct RenderDeviceInfo_t;
class ISource2WorldSession;
class ISceneView;
class IPrerequisite;

DECLARE_POINTER_HANDLE(HSceneViewRenderTarget);

struct ActiveLoop_t
{
	CUtlString m_sLoopName;
	CUtlString m_sAddonName;
	uint32 m_nId;
	KeyValues *m_pLoopOptions;
	bool m_bValid;
};

enum ClientServerMode_t : int32
{
	CLIENTSERVERMODE_NONE = 0,
	CLIENTSERVERMODE_SERVER,
	CLIENTSERVERMODE_CLIENT,
	CLIENTSERVERMODE_LISTENSERVER,
};

struct RHBackColorBuffer_t
{
	ISceneView *m_pView;
	HSceneViewRenderTarget m_hBackColorBuffer;
};


enum LoopModeType_t : int32
{
	LOOP_MODE_TYPE_INVALID = -1,
	LOOP_MODE_TYPE_SIMPLE = 0,
	LOOP_MODE_TYPE_CLIENT,
	LOOP_MODE_TYPE_SERVER,
	LOOP_MODE_TYPE_HOST,

	LOOP_MODE_TYPE_COUNT,
};

enum InputHandlerResult_t : int32
{
  	IHR_INPUT_IGNORED = 0,
  	IHR_INPUT_CONSUMED,
};

abstract_class ILoopType
{
public:
	virtual void AddEngineService(const char* pszName) = 0;
};

abstract_class ILoopModePrerequisiteRegistry : public IPrerequisiteRegistry
{
public:
	virtual void UnregisterPrerequisite( IPrerequisite * ) = 0;
};

abstract_class ILoopMode
{
public:
	virtual bool LoopInit( KeyValues *pKeyValues, ILoopModePrerequisiteRegistry *pRegistry ) = 0;
	virtual void LoopShutdown( void ) = 0;
	virtual void OnLoopActivate( const EngineLoopState_t &state, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher) = 0;
	virtual void OnLoopDeactivate( const EngineLoopState_t &state, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher ) = 0;
	virtual void RegisterEventMap( CEventDispatcher<CEventIDManager_Default> *pEventDispatcher, EventMapRegistrationType_t nRegistrationType) = 0;
	virtual InputHandlerResult_t HandleInputEvent( const InputEvent_t &event, CSplitScreenSlot nSplitScreenPlayerSlot ) = 0;
	virtual RHBackColorBuffer_t AddViewsToSceneSystem( const EngineLoopState_t &state, double flRenderTime, double flRealTime,
		const RenderViewport_t &viewport, const RHBackColorBuffer_t &backColorBuffer ) = 0;
	// AMNOTE: True only while loading a level, unless -favor_consistent_framerate is set; the engine then skips frames while presents are outstanding
	virtual bool unk001( void ) = 0;
};

abstract_class ILoopModeFactory
{
public:
	virtual bool Init( ILoopType *pLoop ) =0;
	virtual void Shutdown( void ) =0;
	virtual ILoopMode *CreateLoopMode( void ) =0;
	virtual void DestroyLoopMode( ILoopMode *pLoopMode ) =0;
	virtual LoopModeType_t GetLoopModeType( void ) const =0;
};

enum EngineServiceActivateType_t
{
	ENGINE_SERVICE_ACTIVATE_NEVER = 0,
	ENGINE_SERVICE_ACTIVATE_ALWAYS,
	ENGINE_SERVICE_ACTIVATE_IF_ADDED_BY_LOOP_MODE,
};

abstract_class IEngineService : public IAppSystem
{
public:
	virtual void		*GetServiceDependencies( void ) = 0;
	virtual const char	*GetName( void ) const = 0;
	virtual EngineServiceActivateType_t ShouldActivate( const char * ) = 0;
	virtual void		OnLoopActivate( const EngineLoopState_t &loopState, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher) = 0;
	virtual void		OnLoopDeactivate( const EngineLoopState_t &loopState, CEventDispatcher<CEventIDManager_Default> *pEventDispatcher) = 0;
	virtual bool		IsActive( void ) const = 0;
	virtual void		SetActive( bool ) = 0;
	virtual void		SetName( const char *pszName ) = 0;
	virtual void		RegisterEventMap( CEventDispatcher<CEventIDManager_Default> *pEventDispatcher, EventMapRegistrationType_t nRegistrationType ) = 0;
	virtual uint16		GetServiceIndex( void ) = 0;
	virtual void		SetServiceIndex( uint16 index ) = 0;
	virtual ~IEngineService();
};

template< class IInterface >
class CBaseEngineService : public CTier4AppSystem< IInterface >
{
public:
	const char *m_pszName;
	bool m_bActive;
	uint16 m_nServiceIndex;
};

abstract_class IEngineServiceMgr : public IAppSystem
{
public:
	virtual void		RegisterEngineService( const char *psServiceName, IEngineService *pService ) = 0;
	virtual void		UnregisterEngineService( const char *pszServiceName, IEngineService *pService ) = 0;
	virtual void		RegisterLoopMode( const char *pLoopModeName, ILoopModeFactory *pLoopModeFactory, void **ppGlobalPointer ) = 0;
	virtual void		UnregisterLoopMode( const char *pLoopModeName, ILoopModeFactory *pLoopModeFactory, void **ppGlobalPointer ) = 0;
	virtual void		SwitchToLoop( const char *pszLoopModeName, KeyValues *pkvLoopOptions, uint32 nId, const char *pszAddonName, bool ) = 0;
	virtual const char		*GetActiveLoopName( void ) const = 0;
	virtual IEngineService	*FindService( const char * ) = 0;
	virtual PlatWindow_t	GetEngineWindow( void ) const = 0;
	virtual SwapChainHandle_t	GetEngineSwapChain( void ) const = 0;
	virtual InputContextHandle_t GetEngineInputContext( void ) const = 0;
	virtual void		*GetEngineDeviceInfo( void ) const = 0;
	virtual int			GetEngineDeviceWidth( void ) const = 0;
	virtual int			GetEngineDeviceHeight( void ) const = 0;
	virtual void		GetEngineSwapChainSize(int* w, int* h) const = 0;
	virtual bool		IsLoopSwitchQueued(void) const = 0;
	virtual bool		IsLoopSwitchRequested(void) const = 0;

	virtual bool		unk101(void) const = 0;

	virtual CEventDispatcher<CEventIDManager_Default>* GetEventDispatcher(void) = 0;
	virtual void		*GetDebugVisualizerMgr( void ) = 0;
	virtual int			GetActiveLoopClientServerMode( void ) const = 0;
	virtual void		PrintStatus( void ) = 0;
	virtual void		GetActiveLoop( ActiveLoop_t *pActiveLoop ) = 0;
	virtual bool		IsLoadingLevel( void ) const = 0;
	virtual bool		IsInGameLoop( void ) const = 0;
	virtual void		OnFrameRenderingFinished( bool, const EventClientOutput_t & ) = 0;
	virtual void		ChangeVideoMode( RenderDeviceInfo_t & ) = 0;
	virtual void		GetVideoModeChange( void ) = 0;
	virtual int			GetAddonCount( void ) const = 0;
	virtual const char	*GetAddon( int ) const = 0;
	virtual bool		IsAddonMounted( const char * ) const = 0;
	virtual const char	*GetAddonsString( void ) const = 0;
	// AMNOTE: Adds the name to a list kept apart from the mounted addons
	virtual void		unk201( const char * ) = 0;
	// AMNOTE: True if unk201 added the name, compared case-insensitively
	virtual bool		unk202( const char * ) = 0;
	// AMNOTE: True if a mounted addon (GetAddon) is missing from the unk201 list
	virtual bool		unk203( void ) = 0;
	virtual void		InstallSwitchLoopModeStatusNotify( ISwitchLoopModeStatusNotify * ) = 0;
	virtual void		UninstallSwitchLoopModeStatusNotify( ISwitchLoopModeStatusNotify * ) = 0;
	virtual void		InstallAddonListChangeNotify( IAddonListChangeNotify * ) = 0;
	virtual void		UninstallAddonListChangeNotify( IAddonListChangeNotify * ) = 0;
	virtual void		StartEngineWatchdogThread( void ) = 0;
	virtual void		AddLogCaptureString( const char * ) = 0;
	virtual void		AddLogCaptureStringV( const char *pFormat, va_list args ) = 0;
	virtual void		AddLogCaptureStringF( const char *pFormat, ... ) = 0;
	// AMNOTE: The number of prerequisites (RegisterPrerequisite) the queued loop switch still waits for
	virtual int			unk301( void ) = 0;
	virtual void		ExitMainLoop( void ) = 0;
	virtual void		RegisterPrerequisite( IPrerequisite * ) = 0;
	
	// Same methods as ILocalize
	virtual const char *LookupLocalizationToken(const char *tokenName) = 0;

	// Same methods as IVEngineServer2 
	virtual void		SetFrameTimeAmnesty( const char *amnesty, int, float frametime ) = 0;
	virtual const char *GetFrameTimeAmnesty( bool check_cvar ) = 0;
	virtual void		SetFramePerformanceTag( const char *pszTag, int nValue, int, float flDuration ) = 0;
#ifdef PLATFORM_LINUX
	virtual void		UnregisterPrerequisite( IPrerequisite * ) = 0;
#endif
};

#endif // IENGINESERVICEMGR_H
