//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef FASTTIMER_H
#define FASTTIMER_H
#ifdef _WIN32
#pragma once
#endif

#include <assert.h>
#include "platform.h"

#ifdef COMPILER_MSVC64
extern "C"
{
	unsigned __int64 __rdtsc();
}

#pragma intrinsic(__rdtsc)
#endif

class CCycleCount
{
	friend class CFastTimer;

public:
					CCycleCount();
					CCycleCount( uint64 cycles );

	void			Sample();	// Sample the clock. This takes about 34 clocks to execute (or 26,000 calls per millisecond on a P900).

	void			Init();		// Set to zero.
	void			Init( uint64 cycles );
	bool			IsLessThan( CCycleCount const &other ) const;					// Compare two counts.

	// Convert to other time representations. These functions are slow, so it's preferable to call them
	// during display rather than inside a timing block.
	unsigned long	GetCycles()  const;
	uint64			GetLongCycles() const;

	double			GetMillisecondsF() const;

	CCycleCount&	operator+=( CCycleCount const &other );

	// dest = rSrc1 + rSrc2
	static void		Add( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest );	// Add two samples together.
	
	// dest = rSrc1 - rSrc2
	static void		Sub( CCycleCount const &rSrc1, CCycleCount const &rSrc2, CCycleCount &dest );	// Add two samples together.

	static uint64	GetTimestamp();

	uint64			m_Int64;
};

class CFastTimer
{
public:
	// These functions are fast to call and should be called from your sampling code.
	void				Start();
	void				End();

	const CCycleCount &	GetDuration() const;	// Get the elapsed time between Start and End calls.
	CCycleCount 		GetDurationInProgress() const; // Call without ending. Not that cheap.

private:
	CCycleCount	m_Duration;
#ifdef DEBUG_FASTTIMER
	bool m_bRunning;		// Are we currently running?
#endif
};


// -------------------------------------------------------------------------- // 
// CCycleCount inlines.
// -------------------------------------------------------------------------- // 





#ifdef COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4189) // warning C4189: local variable is initialized but not referenced
#endif


#ifdef COMPILER_MSVC
#pragma warning(pop)
#endif














// -------------------------------------------------------------------------- // 
// CFastTimer inlines.
// -------------------------------------------------------------------------- // 







#endif // FASTTIMER_H
