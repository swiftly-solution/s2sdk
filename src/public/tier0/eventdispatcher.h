#ifndef EVENTDISPATCHER_H
#define EVENTDISPATCHER_H

#ifdef _WIN32
#pragma once
#endif

#include "utldelegate.h"
#include "utlmap.h"
#include "utlstring.h"
#include "threadtools.h"
#include "public/schemalib/schemaclassinfo.h"

enum EventMapRegistrationType_t : int32
{
	EVENT_MAP_REGISTER = 0,
	EVENT_MAP_UNREGISTER,
};

class CEventDispatcher_Base
{
public:
	struct EventListenerInfo_t
	{
		CUtlAbstractDelegate m_Delegate;
		const char *m_pszName;
		int32 m_nPriority;
		uint8 m_nDelegateParamCount;
		bool m_bDelegateReturnsVoid;
	};

	struct DelegateIterator_Base_t
	{
		const CUtlVector< EventListenerInfo_t > *pListeners;
		CUtlVectorFixedGrowable< int, 4 > skipListeners;
		int nCurrent;
		DelegateIterator_Base_t *pNext;
		bool bIteratingForward;
		bool bIsInListenerTelemetryScope;
	};

protected:
	DLL_CLASS_IMPORT CEventDispatcher_Base();

	// Listeners are kept sorted from the highest nPriority, in registration order within a priority
	DLL_CLASS_IMPORT void RegisterEventListener_Base( const CUtlAbstractDelegate &delegate, uint32 nDelegateParamCount, bool bDelegateReturnsVoid, CUtlVector< EventListenerInfo_t > &listeners, int nPriority, const char *pszName );
	DLL_CLASS_IMPORT bool UnregisterEventListener_Base( const CUtlAbstractDelegate &delegate, CUtlVector< EventListenerInfo_t > &listeners );

	DLL_CLASS_IMPORT void GetListenerNames_Base( const CUtlVector< EventListenerInfo_t > &listeners, CUtlVector< CUtlString > *pResult );

	// Listeners registered or unregistered during an iteration are handled by every active iterator
	DLL_CLASS_IMPORT bool IterateListeners_Start( DelegateIterator_Base_t &iterationStruct, bool bForward );
	DLL_CLASS_IMPORT const EventListenerInfo_t *IterateListener( DelegateIterator_Base_t &iterationStruct );
	DLL_CLASS_IMPORT void IterateListeners_End( DelegateIterator_Base_t &iterationStruct );

	DLL_CLASS_IMPORT int FindInsertionPoint( const CUtlVector< EventListenerInfo_t > &listeners, int nPriority );

	CThreadFastMutex m_Lock;
	DelegateIterator_Base_t *m_pActiveIterators;
};

class CEventID_SchemaBinding
{
	int8 unused;
};

class CEventIDManager_SchemaBinding : public CEventID_SchemaBinding
{
};

class CEventIDManager_Default : public CEventIDManager_SchemaBinding
{
};

//-----------------------------------------------------------------------------
// Events are identified by the schema binding of their struct in the global type scope:
//	g_pSchemaSystem->GlobalTypeScope()->FindDeclaredClass( "EventServerAdvanceTick_t" ).Get()
//
// A listener takes the event struct by const reference, or nothing. One that returns bool
// stops the event from reaching the listeners after it by returning false.
// Listeners with a higher priority are called first.
//-----------------------------------------------------------------------------
template < typename EVENT_ID_MANAGER >
class CEventDispatcher_Identified : public CEventDispatcher_Base
{
public:
	typedef const CSchemaClassInfo *EventID_t;

	struct DelegateIterator_t : public DelegateIterator_Base_t
	{
		EventID_t EventID;
	};

	void RegisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, uint32 nDelegateParamCount, bool bDelegateReturnsVoid, EventID_t EventID, int nPriority = 0, const char *pszName = nullptr );
	void UnregisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, EventID_t EventID );

	template < typename EVENT >
	void RegisterEventListener( EventID_t EventID, CUtlDelegate< void ( const EVENT & ) > delegate, int nPriority = 0, const char *pszName = nullptr )
	{
		RegisterEventListener_Abstract( delegate.GetAbstractDelegate(), 1, true, EventID, nPriority, pszName );
	}
	template < typename EVENT >
	void RegisterEventListener( EventID_t EventID, CUtlDelegate< bool ( const EVENT & ) > delegate, int nPriority = 0, const char *pszName = nullptr )
	{
		RegisterEventListener_Abstract( delegate.GetAbstractDelegate(), 1, false, EventID, nPriority, pszName );
	}
	void RegisterEventListener( EventID_t EventID, CUtlDelegate< void () > delegate, int nPriority = 0, const char *pszName = nullptr )
	{
		RegisterEventListener_Abstract( delegate.GetAbstractDelegate(), 0, true, EventID, nPriority, pszName );
	}
	void RegisterEventListener( EventID_t EventID, CUtlDelegate< bool () > delegate, int nPriority = 0, const char *pszName = nullptr )
	{
		RegisterEventListener_Abstract( delegate.GetAbstractDelegate(), 0, false, EventID, nPriority, pszName );
	}

	template < typename DELEGATE >
	void UnregisterEventListener( EventID_t EventID, DELEGATE delegate )
	{
		UnregisterEventListener_Abstract( delegate.GetAbstractDelegate(), EventID );
	}

	bool HasListeners( EventID_t EventID );
	void GetListenerNames( EventID_t EventID, CUtlVector< CUtlString > *pResult );

	// Locks the dispatcher and returns true when the event has listeners,
	// then IterateListeners_End must be called once done with them
	bool IterateListeners_Start( DelegateIterator_t &iterationStruct, EventID_t EventID, bool bForward = true );
	const EventListenerInfo_t *IterateListener( DelegateIterator_t &iterationStruct ) { return CEventDispatcher_Base::IterateListener( iterationStruct ); }
	void IterateListeners_End( DelegateIterator_t &iterationStruct );

	static bool IsValidEventID( EventID_t EventID ) { return EventID != nullptr && ( EventID->m_nClassFlags1 & SCHEMA_CF1_GLOBAL_TYPE_SCOPE ) != 0; }

protected:
	// Requires m_Lock
	CUtlVector< EventListenerInfo_t > *FindListeners( EventID_t EventID );

	CUtlOrderedMap< EventID_t, CUtlVector< EventListenerInfo_t >, CDefLess< EventID_t >, unsigned int > m_EventListenerMap;
};

template < typename EVENT_ID_MANAGER >
class CEventDispatcher : public CEventDispatcher_Identified< EVENT_ID_MANAGER >
{
};

template < typename EVENT_ID_MANAGER >
inline CUtlVector< CEventDispatcher_Base::EventListenerInfo_t > *CEventDispatcher_Identified< EVENT_ID_MANAGER >::FindListeners( EventID_t EventID )
{
	auto i = m_EventListenerMap.Find( EventID );
	return m_EventListenerMap.IsValidIndex( i ) ? &m_EventListenerMap.Element( i ) : nullptr;
}

template < typename EVENT_ID_MANAGER >
void CEventDispatcher_Identified< EVENT_ID_MANAGER >::RegisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, uint32 nDelegateParamCount, bool bDelegateReturnsVoid, EventID_t EventID, int nPriority, const char *pszName )
{
	if ( !IsValidEventID( EventID ) )
		return;

	m_Lock.Lock();

	auto i = m_EventListenerMap.Find( EventID );
	if ( !m_EventListenerMap.IsValidIndex( i ) )
	{
		i = m_EventListenerMap.Insert( EventID );

		// The insertion can move the other listener lists
		for ( DelegateIterator_Base_t *pIterator = m_pActiveIterators; pIterator; pIterator = pIterator->pNext )
		{
			if ( pIterator->pListeners )
				pIterator->pListeners = FindListeners( static_cast< DelegateIterator_t * >( pIterator )->EventID );
		}
	}

	RegisterEventListener_Base( delegate, nDelegateParamCount, bDelegateReturnsVoid, m_EventListenerMap.Element( i ), nPriority, pszName );

	m_Lock.Unlock();
}

template < typename EVENT_ID_MANAGER >
void CEventDispatcher_Identified< EVENT_ID_MANAGER >::UnregisterEventListener_Abstract( const CUtlAbstractDelegate &delegate, EventID_t EventID )
{
	if ( !IsValidEventID( EventID ) )
		return;

	m_Lock.Lock();

	CUtlVector< EventListenerInfo_t > *pListeners = FindListeners( EventID );
	if ( pListeners )
		UnregisterEventListener_Base( delegate, *pListeners );

	m_Lock.Unlock();
}

template < typename EVENT_ID_MANAGER >
bool CEventDispatcher_Identified< EVENT_ID_MANAGER >::HasListeners( EventID_t EventID )
{
	if ( !IsValidEventID( EventID ) )
		return false;

	m_Lock.Lock();

	CUtlVector< EventListenerInfo_t > *pListeners = FindListeners( EventID );
	bool bHasListeners = pListeners && pListeners->Count() > 0;

	m_Lock.Unlock();

	return bHasListeners;
}

template < typename EVENT_ID_MANAGER >
void CEventDispatcher_Identified< EVENT_ID_MANAGER >::GetListenerNames( EventID_t EventID, CUtlVector< CUtlString > *pResult )
{
	if ( !IsValidEventID( EventID ) )
		return;

	m_Lock.Lock();

	CUtlVector< EventListenerInfo_t > *pListeners = FindListeners( EventID );
	if ( pListeners )
		GetListenerNames_Base( *pListeners, pResult );

	m_Lock.Unlock();
}

template < typename EVENT_ID_MANAGER >
bool CEventDispatcher_Identified< EVENT_ID_MANAGER >::IterateListeners_Start( DelegateIterator_t &iterationStruct, EventID_t EventID, bool bForward )
{
	if ( !IsValidEventID( EventID ) )
		return false;

	iterationStruct.EventID = EventID;

	m_Lock.Lock();

	iterationStruct.pListeners = FindListeners( EventID );
	if ( CEventDispatcher_Base::IterateListeners_Start( iterationStruct, bForward ) )
		return true;

	CEventDispatcher_Base::IterateListeners_End( iterationStruct );
	m_Lock.Unlock();

	return false;
}

template < typename EVENT_ID_MANAGER >
void CEventDispatcher_Identified< EVENT_ID_MANAGER >::IterateListeners_End( DelegateIterator_t &iterationStruct )
{
	CEventDispatcher_Base::IterateListeners_End( iterationStruct );
	m_Lock.Unlock();
}

#endif // EVENTDISPATCHER_H
