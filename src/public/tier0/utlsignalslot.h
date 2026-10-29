#ifndef UTLSIGNALSLOT_H
#define UTLSIGNALSLOT_H

#ifdef _WIN32
#pragma once
#endif

#include "threadtools.h"
#include "utldelegate.h"
#include "utlvector.h"

// AMNOTE: Layout only, to derive the engine classes that are slots from it. The engine connects signallers
// to a slot, like a net channel when a net message handler is registered for it, and the slot's destructor
// calls each of their delegates so they drop it.
class CUtlSlot
{
public:
	CUtlSlot() = delete;
	CUtlSlot( const CUtlSlot & ) = delete;

private:
	CThreadFastMutex m_Mutex;

	// AMNOTE: Points at the start of each connected signaller, the delegate it gets called with on the slot's destruction
	CUtlVector<CUtlDelegate<void( CUtlSlot * )> *> m_ConnectedSignallers;
};

#endif // UTLSIGNALSLOT_H
