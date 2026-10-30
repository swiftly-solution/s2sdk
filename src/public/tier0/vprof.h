//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Real-Time Hierarchical Profiling
//
// $NoKeywords: $
//=============================================================================//

#ifndef VPROF_H
#define VPROF_H

#include "dbg.h"
#include "fasttimer.h"
#include "l2cache.h"
#include "threadtools.h"
#include "utlvector.h"

// VProf is enabled by default in all configurations.
#define VPROF_ENABLED

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4251)
#endif

// enable this to get detailed nodes beneath budget
// #define VPROF_LEVEL 1

//-----------------------------------------------------------------------------
//
// Profiling instrumentation macros
//

#define MAXCOUNTERS 256


#ifdef VPROF_ENABLED

#define VPROF_VTUNE_GROUP

#define	VPROF( name )						VPROF_(name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, false, 0)
#define	VPROF_ASSERT_ACCOUNTED( name )		VPROF_(name, 1, VPROF_BUDGETGROUP_OTHER_UNACCOUNTED, true, 0)
#define	VPROF_( name, detail, group, bAssertAccounted, budgetFlags )		VPROF_##detail(name,group, bAssertAccounted, budgetFlags)

#define VPROF_BUDGET( name, group )					VPROF_BUDGET_FLAGS(name, group, BUDGETFLAG_OTHER)
#define VPROF_BUDGET_FLAGS( name, group, flags )	VPROF_(name, 0, group, false, flags)

#define VPROF_SCOPE_BEGIN( tag )	do { VPROF( tag )
#define VPROF_SCOPE_END()			} while (0)

#define VPROF_ONLY( expression )	expression

#define VPROF_ENTER_SCOPE( name )			VProf_EnterScopeAdHoc( name, { __FILE__, __LINE__, __func__ } )
#define VPROF_EXIT_SCOPE()					VProf_ExitScope()

#define VPROF_BUDGET_GROUP_ID_UNACCOUNTED 0


// Budgetgroup flags. These are used with VPROF_BUDGET_FLAGS.
// These control which budget panels the groups show up in.
// If a budget group uses VPROF_BUDGET, it gets the default 
// which is BUDGETFLAG_OTHER.
#define BUDGETFLAG_CLIENT	(1<<0)		// Shows up in the client panel.
#define BUDGETFLAG_SERVER	(1<<1)		// Shows up in the server panel.
#define BUDGETFLAG_OTHER	(1<<2)		// Unclassified (the client shows these but the dedicated server doesn't).
#define BUDGETFLAG_HIDDEN	(1<<15)
#define BUDGETFLAG_ALL		0xFFFF


// NOTE: You can use strings instead of these defines. . they are defined here and added
// in vprof.cpp so that they are always in the same order.
#define VPROF_BUDGETGROUP_OTHER_UNACCOUNTED			_T("Unaccounted")
#define VPROF_BUDGETGROUP_WORLD_RENDERING			_T("World Rendering")
#define VPROF_BUDGETGROUP_DISPLACEMENT_RENDERING	_T("Displacement_Rendering")
#define VPROF_BUDGETGROUP_GAME						_T("Game")
#define VPROF_BUDGETGROUP_NPCS						_T("NPCs")
#define VPROF_BUDGETGROUP_SERVER_ANIM				_T("Server Animation")
#define VPROF_BUDGETGROUP_PHYSICS					_T("Physics")
#define VPROF_BUDGETGROUP_STATICPROP_RENDERING		_T("Static_Prop_Rendering")
#define VPROF_BUDGETGROUP_MODEL_RENDERING			_T("Other_Model_Rendering")
#define VPROF_BUDGETGROUP_MODEL_FAST_PATH_RENDERING _T("Fast Path Model Rendering")
#define VPROF_BUDGETGROUP_BRUSHMODEL_RENDERING		_T("Brush_Model_Rendering")
#define VPROF_BUDGETGROUP_SHADOW_RENDERING			_T("Shadow_Rendering")
#define VPROF_BUDGETGROUP_DETAILPROP_RENDERING		_T("Detail_Prop_Rendering")
#define VPROF_BUDGETGROUP_PARTICLE_RENDERING		_T("Particle/Effect_Rendering")
#define VPROF_BUDGETGROUP_ROPES						_T("Ropes")
#define VPROF_BUDGETGROUP_DLIGHT_RENDERING			_T("Dynamic_Light_Rendering")
#define VPROF_BUDGETGROUP_OTHER_NETWORKING			_T("Networking")
#define VPROF_BUDGETGROUP_CLIENT_ANIMATION			_T("Client_Animation")
#define VPROF_BUDGETGROUP_OTHER_SOUND				_T("Sound")
#define VPROF_BUDGETGROUP_OTHER_VGUI				_T("VGUI")
#define VPROF_BUDGETGROUP_OTHER_FILESYSTEM			_T("FileSystem")
#define VPROF_BUDGETGROUP_PREDICTION				_T("Prediction")
#define VPROF_BUDGETGROUP_INTERPOLATION				_T("Interpolation")
#define VPROF_BUDGETGROUP_SWAP_BUFFERS				_T("Swap_Buffers")
#define VPROF_BUDGETGROUP_PLAYER					_T("Player")
#define VPROF_BUDGETGROUP_OCCLUSION					_T("Occlusion")
#define VPROF_BUDGETGROUP_OVERLAYS					_T("Overlays")
#define VPROF_BUDGETGROUP_TOOLS						_T("Tools")
#define VPROF_BUDGETGROUP_LIGHTCACHE				_T("Light_Cache")
#define VPROF_BUDGETGROUP_DISP_HULLTRACES			_T("Displacement_Hull_Traces")
#define VPROF_BUDGETGROUP_TEXTURE_CACHE				_T("Texture_Cache")
#define VPROF_BUDGETGROUP_PARTICLE_SIMULATION		_T("Particle Simulation")
#define VPROF_BUDGETGROUP_SHADOW_DEPTH_TEXTURING	_T("Flashlight Shadows")
#define VPROF_BUDGETGROUP_CLIENT_SIM				_T("Client Simulation") // think functions, tempents, etc.
#define VPROF_BUDGETGROUP_STEAM						_T("Steam") 
#define VPROF_BUDGETGROUP_CVAR_FIND					_T("Cvar_Find") 
#define VPROF_BUDGETGROUP_CLIENTLEAFSYSTEM			_T("ClientLeafSystem")
#define VPROF_BUDGETGROUP_JOBS_COROUTINES			_T("Jobs/Coroutines")
	
//-------------------------------------

#ifndef VPROF_LEVEL
#define VPROF_LEVEL 0
#endif

#define	VPROF_0(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<0, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });

#if VPROF_LEVEL > 0 
#define	VPROF_1(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<1, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_1(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 1 
#define	VPROF_2(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<2, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_2(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 2 
#define	VPROF_3(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<3, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_3(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

#if VPROF_LEVEL > 3 
#define	VPROF_4(name,group,assertAccounted,budgetFlags)	VProfScopeHelper<4, assertAccounted> vprofHelper_(name, group, budgetFlags, { __FILE__, __LINE__, __func__ });
#else
#define	VPROF_4(name,group,assertAccounted,budgetFlags)	((void)0)
#endif

//-------------------------------------

#ifdef _MSC_VER
#define VProfCode( code ) \
	if ( 0 ) \
		; \
	else \
	{ \
	VPROF( __FUNCTION__ ": " #code ); \
		code; \
	}
#else
#define VProfCode( code ) \
	if ( 0 ) \
		; \
	else \
	{ \
		VPROF( #code ); \
		code; \
	} 
#endif


//-------------------------------------

#define VPROF_INCREMENT_COUNTER(name,amount)			do { static CVProfCounter _counter( name ); _counter.Increment( amount ); } while( 0 )
#define VPROF_INCREMENT_GROUP_COUNTER(name,group,amount)			do { static CVProfCounter _counter( name, group ); _counter.Increment( amount ); } while( 0 )

#else

#define	VPROF( name )									((void)0)
#define	VPROF_ASSERT_ACCOUNTED( name )					((void)0)
#define	VPROF_( name, detail, group, bAssertAccounted, budgetFlags )	((void)0)
#define VPROF_BUDGET( name, group )						((void)0)
#define VPROF_BUDGET_FLAGS( name, group, flags )		((void)0)

#define VPROF_SCOPE_BEGIN( tag )	do {
#define VPROF_SCOPE_END()			} while (0)

#define VPROF_ONLY( expression )	((void)0)

#define VPROF_ENTER_SCOPE( name )
#define VPROF_EXIT_SCOPE()

#define VPROF_INCREMENT_COUNTER(name,amount)			((void)0)
#define VPROF_INCREMENT_GROUP_COUNTER(name,group,amount)	((void)0)

#define VPROF_TEST_SPIKE( msec )	((void)0)

#define VProfCode( code ) code

#endif
 
//-----------------------------------------------------------------------------

#ifdef VPROF_ENABLED

class CUtlSourceLocation
{
public:
	CUtlSourceLocation( const char *file_path, uint64 file_line, const char *method_name ) :
		m_FilePath( file_path ), m_FileLine( file_line ), m_MethodName( method_name )
	{}

private:
	const char *m_FilePath;
	uint64 m_FileLine;
	const char *m_MethodName;
};

// Enters a scope of the Unaccounted budget group, on the profiled thread only
PLATFORM_INTERFACE void VProf_EnterScopeAdHoc( const char *pszName, CUtlSourceLocation location );
PLATFORM_INTERFACE void VProf_ExitScope();

//-----------------------------------------------------------------------------
// VProf lite: the time spent in each budget group, sampled per frame and per second
//-----------------------------------------------------------------------------

// Times are in seconds
struct VProfLitePercentiles_t
{
	float m_flAvg;
	float m_flP25;
	float m_flP50;
	float m_flP95;
	float m_flP99;
};

struct VProfLiteBudgetGroupData_t
{
	int m_nBudgetGroupID;
	const char *m_pszName;
	int m_nNumSamples;
	// One second intervals
	int m_nNumIntervals;
	// Over the frames where the group had time, then over all the frames
	VProfLitePercentiles_t m_Active;
	VProfLitePercentiles_t m_All;
	// Over the highest frame time of each interval
	VProfLitePercentiles_t m_IntervalMaxActive;
	VProfLitePercentiles_t m_IntervalMaxAll;
	float m_flMax;
};

struct VProfLiteHistoricalData_t
{
	int m_nExcludedFrames;
	CUtlVector< VProfLiteBudgetGroupData_t > m_BudgetGroups;
};

// The current frame's time in a budget group so far, in seconds
struct VProfLiteFrameStats_t
{
	float m_flTime;
	// Without the groups entered from this one
	float m_flExclusiveTime;
};

PLATFORM_INTERFACE void VProfLite_SetEnabled( bool bEnabled );
PLATFORM_INTERFACE bool VProfLite_GetEnabled();
// Clears the recorded data
PLATFORM_INTERFACE void VProfLite_Clear();
// Bases the percentiles on the time spent in the groups without the ones entered from them
PLATFORM_INTERFACE void VProfLite_SetExclusivePercentiles( bool bExclusive );
// AMNOTE: nFrameSampling: 2 records the frame's times, 1 counts it as excluded from the analysis
PLATFORM_INTERFACE void VProfLite_NextFrame( int nFrameSampling );
PLATFORM_INTERFACE VProfLiteFrameStats_t VProfLite_GetBudgetGroupCurrentFrameStats( int nBudgetGroupID, uint64 nCurrentTicks );
PLATFORM_INTERFACE void VProfLite_GetHistoricalData( VProfLiteHistoricalData_t *pData );
// Takes the dedicated server's sleep out of the frame times, keeping the time it overslept
PLATFORM_INTERFACE void VProfLite_DedicatedServerSleepBegin();
PLATFORM_INTERFACE void VProfLite_DedicatedServerSleepEnd( double flOversleepTime );

struct VProfReportSettings_t;
struct CVProfSummingContext;
struct VProfBudgetGroupCallSite;

typedef void (*VProfExitScopeCB)();

//-----------------------------------------------------------------------------
//
// A node in the call graph hierarchy
//

class PLATFORM_CLASS CVProfNode 
{
	friend class CVProfRecorder;
	friend class CVProfile;

public:
	CVProfNode( const char *pszName, CVProfNode *pParent, VProfBudgetGroupCallSite &pBudgetGroupName, const CUtlSourceLocation &location );
	~CVProfNode();

	CVProfNode *GetVParent() { return m_pParent; }
	const CVProfNode *GetVParent() const { return m_pParent; }
	CVProfNode *GetVSibling() { return m_pSibling; }
	const CVProfNode *GetVPrevSibling() const;
	const CVProfNode *GetVSibling() const { return m_pSibling; }
	CVProfNode *GetVChild() { return m_pChild; }
	const CVProfNode *GetVChild() const { return m_pChild; }
	
	void MarkFrame();
	void ResetPeak();
	
	void Pause();
	void Resume();
	void Reset();

	void EnterScope();
	bool ExitScope();

	CVProfNode *FindOrCreateChild( const char *, VProfBudgetGroupCallSite &, const CUtlSourceLocation & );

	const char *GetName() const { return m_pszName; }

	// Only used by the record/playback stuff.
	void SetBudgetGroupID( int id ) { m_BudgetGroupID = id; }
	int GetBudgetGroupID() const { return m_BudgetGroupID; }

	// Times are in milliseconds
	int	GetCurCalls() const { return m_nCurFrameCalls; }
	double GetCurTime() const { return CyclesToMilliseconds( m_CurFrameTime ); }
	int GetPrevCalls() const { return m_nPrevFrameCalls; }
	double GetPrevTime() const { return CyclesToMilliseconds( m_PrevFrameTime ); }
	int	GetTotalCalls() const { return m_nTotalCalls; }
	double GetTotalTime() const { return CyclesToMilliseconds( m_TotalTime ); }
	double GetPeakTime() const { return CyclesToMilliseconds( m_PeakTime ); }

	const CUtlSourceLocation &GetSourceLocation() const { return m_SourceLocation; }

	double GetCurTimeLessChildren() const;
	double GetPrevTimeLessChildren() const;
	double GetTotalTimeLessChildren() const;

	void ClearPrevTime() { m_PrevFrameTime.Init(); m_unk301 = 0; }

	// Not used in the common case...
	void SetCurFrameTime( unsigned long milliseconds );
	
	void SetClientData( int iClientData ) { m_iClientData = iClientData; }
	int GetClientData() const { return m_iClientData; }

// Used by vprof record/playback.
protected:
	CVProfNode( const char *, VProfBudgetGroupCallSite &, double, const CUtlSourceLocation & );

	void SetUniqueNodeID( int id ) { m_iUniqueNodeID = id; }
	int GetUniqueNodeID() const { return m_iUniqueNodeID; }

private:
	static double CyclesToMilliseconds( const CCycleCount &cycles ) { return cycles.GetLongCycles() * ( 1000.0 / Plat_CPUTickFrequency() ); }

	const tchar *m_pszName;
	CFastTimer	m_Timer;
	// AMNOTE: The first is zeroed whenever m_Timer starts, tier0 never reads either
	int			m_unk101;
	// AMNOTE: Only the constructor writes it, zeroing it together with m_unk101, so the two may be one 8-byte field
	int			m_unk102;

	int			m_nRecursions;
	
	unsigned	m_nCurFrameCalls;
	CCycleCount	m_CurFrameTime;
	// AMNOTE: tier0 only zeroes it or copies it along with the time before it, as with the ones below
	uint64		m_unk201;
	
	unsigned	m_nPrevFrameCalls;
	CCycleCount	m_PrevFrameTime;
	uint64		m_unk301;

	unsigned	m_nTotalCalls;
	CCycleCount	m_TotalTime;
	uint64		m_unk401;

	CCycleCount	m_PeakTime;
	uint64		m_unk501;

	unsigned	m_nMergedCurFrameCalls;
	CCycleCount	m_MergedCurFrameTime;

	CVProfNode *m_pParent;
	CVProfNode *m_pChild;
	CVProfNode *m_pSibling;

	int m_BudgetGroupID;
	
	int m_iClientData;
	int m_iUniqueNodeID;

	CUtlSourceLocation m_SourceLocation;
};

inline const CVProfNode *CVProfNode::GetVPrevSibling() const
{
	if ( !m_pParent )
		return NULL;

	const CVProfNode *pNode = m_pParent->m_pChild;
	while ( pNode && pNode->m_pSibling != this )
		pNode = pNode->m_pSibling;

	return pNode;
}

inline double CVProfNode::GetCurTimeLessChildren() const
{
	double result = GetCurTime();
	for ( const CVProfNode *pChild = m_pChild; pChild; pChild = pChild->m_pSibling )
		result -= pChild->GetCurTime();

	return result;
}

inline double CVProfNode::GetPrevTimeLessChildren() const
{
	double result = GetPrevTime();
	for ( const CVProfNode *pChild = m_pChild; pChild; pChild = pChild->m_pSibling )
		result -= pChild->GetPrevTime();

	return result;
}

inline double CVProfNode::GetTotalTimeLessChildren() const
{
	double result = GetTotalTime();
	for ( const CVProfNode *pChild = m_pChild; pChild; pChild = pChild->m_pSibling )
		result -= pChild->GetTotalTime();

	return result;
}

//-----------------------------------------------------------------------------
//
// Coordinator and root node of the profile hierarchy tree
//

enum VProfReportType_t
{
	VPRT_SUMMARY									= ( 1 << 0 ),
	VPRT_HIERARCHY									= ( 1 << 1 ),
	VPRT_HIERARCHY_TIME_PER_FRAME_AND_COUNT_ONLY	= ( 1 << 2 ),
	VPRT_LIST_BY_TIME								= ( 1 << 3 ),
	VPRT_LIST_BY_TIME_LESS_CHILDREN					= ( 1 << 4 ),
	VPRT_LIST_BY_AVG_TIME							= ( 1 << 5 ),	
	VPRT_LIST_BY_AVG_TIME_LESS_CHILDREN				= ( 1 << 6 ),
	VPRT_LIST_BY_PEAK_TIME							= ( 1 << 7 ),
	VPRT_LIST_BY_PEAK_OVER_AVERAGE					= ( 1 << 8 ),
	VPRT_LIST_TOP_ITEMS_ONLY						= ( 1 << 9 ),

	VPRT_FULL = (0xffffffff & ~(VPRT_HIERARCHY_TIME_PER_FRAME_AND_COUNT_ONLY|VPRT_LIST_TOP_ITEMS_ONLY)),
};

enum CounterGroup_t
{
	COUNTER_GROUP_DEFAULT=0,
	COUNTER_GROUP_NO_RESET,				// The engine doesn't reset these counters. Usually, they are used 
										// like global variables that can be accessed across modules.
	COUNTER_GROUP_TEXTURE_GLOBAL,		// Global texture usage counters (totals for what is currently in memory).
	COUNTER_GROUP_TEXTURE_PER_FRAME		// Per-frame texture usage counters.
}; 

// A node name's times summed over the profile, in milliseconds
struct VProfDataReportItem_t
{
	const char *m_pszName;
	int m_nCalls;
	double m_flCallsPerFrame;
	double m_flTotalTime;
	double m_flTotalTimeLessChildren;
	// Of the root's total time, up to 100
	double m_flTotalTimePercent;
	double m_flTotalTimeLessChildrenPercent;
	double m_flTimePerFrame;
	double m_flAvgTime;
	double m_flAvgTimeLessChildren;
	double m_flPeakTime;
};

class PLATFORM_CLASS CVProfile 
{
public:
	CVProfile();
	~CVProfile();

	int AssignNextTimespanId();
	
	void Term();

	void CalcBudgetGroupTimes_Recursive( CVProfNode *, unsigned int *, int, float );

	void EnterScopeNodeTargetThread( CVProfNode *node );
	void ExitScopeTargetThread();

	static VProfExitScopeCB EnterScopeBackgroundThread( const char *pszName, VProfBudgetGroupCallSite &budgetGroup, const CUtlSourceLocation &location );
	static void ExitScopeBackgroundThread();
	CVProfNode *GetBackgroundRoot();
	CVProfNode *GetBackgroundRootNoCreate() const { return m_pBackgroundRoot; }
	CAtomicMutex &GetBackgroundRootMutex() { return m_BackgroundRootMutex; }
	void MergeBackgroundTrees();
	void SetProfileAllThreads( bool bProfileAllThreads );
	bool IsProfilingAllThreads() const { return m_bProfileAllThreads; }

	ThreadId_t GetTargetThreadId() { return m_TargetThreadId; }
	void SetTargetThreadId( ThreadId_t id ) { m_TargetThreadId = id; }
	bool InTargetThread() { return m_TargetThreadId == ThreadGetCurrentId(); }

#ifdef PLATFORM_WINDOWS
	// Enters the scope on the target thread only, while profiling
	void EnterScopeTargetThread( const char *pszName, bool, VProfBudgetGroupCallSite &budgetGroup, const CUtlSourceLocation &location );
	// The same, for callers that checked both
	void EnterScopeTargetThreadChecked( const char *pszName, bool, VProfBudgetGroupCallSite &budgetGroup, const CUtlSourceLocation &location );
	void ExitScopeTargetThreadChecked();
#endif

	bool AtRoot() const { return m_fAtRoot; }
	CVProfNode *GetRoot() { return &m_Root; }
	CVProfNode *GetCurrentNode() { return m_pCurNode; }

	int NumFramesSampled() const { return m_nFrames; }

	// Times are in milliseconds
	double GetTimeLastFrame() const { return m_Root.GetCurTime(); }
	double GetPeakFrameTime() const { return m_Root.GetPeakTime(); }
	double GetTotalTimeSampled() const { return m_Root.GetTotalTime(); }
	double GetTotalSecondsSampled() const { return m_Root.GetTotalTime() / 1000.0; }
	// Excludes the paused time
	double GetTotalWallClockSecondsSampled();

	void ResetPeaks();

	// Sums the times of each node name, over the background threads too
	void GenerateDataReport( CUtlVector< VProfDataReportItem_t > *pReport );

	//
	// Queries
	//

#ifdef VPROF_VTUNE_GROUP
#	define MAX_GROUP_STACK_DEPTH 1024
#endif
	
	void SetOutputStream( void (*)(const char *, ...) );

	CVProfNode *FindNode( CVProfNode *pStartNode, const char *pszNode );

	void OutputReport( const VProfReportSettings_t &, const char *pszStartNode, int budgetGroupID = -1 );
	void OutputReport( const char *pszStartNode = NULL, int budgetGroupID = -1 );

	static int GetNumBudgetGroups( void );
	static const char *GetBudgetGroupName( int budgetGroupID );
	static int GetBudgetGroupFlags( int budgetGroupID );	// Returns a combination of BUDGETFLAG_ defines.
	static void GetBudgetGroupColor( int budgetGroupID, int &r, int &g, int &b, int &a );

	static int BudgetGroupNameToBudgetGroupID( const char *pBudgetGroupName );
	static int BudgetGroupNameToBudgetGroupID( const char *pBudgetGroupName, int budgetFlagsToORIn );
	// Returns -1 for a budget group that doesn't exist
	static int BudgetGroupNameToBudgetGroupIDNoCreate( const char *pBudgetGroupName ) { return FindBudgetGroupName( pBudgetGroupName ); }

#ifdef VPROF_VTUNE_GROUP
	void EnableVTuneGroup( const char *pGroupName ) { m_nVTuneGroupID = BudgetGroupNameToBudgetGroupID( pGroupName, BUDGETFLAG_OTHER ); m_bVTuneGroupEnabled = true; }
	void DisableVTuneGroup() { m_bVTuneGroupEnabled = false; }
#ifdef PLATFORM_WINDOWS
	void PushGroup( int nGroupID );
	void PopGroup();
#endif
#endif

	void HideBudgetGroup( int budgetGroupID, bool bHide = true );

	uint64 *FindOrCreateCounter( const char *pName, CounterGroup_t eCounterGroup = COUNTER_GROUP_DEFAULT );
	void ResetCounters( CounterGroup_t eCounterGroup );
	
	int GetNumCounters( void ) const;
	
	uint64 GetCounterValue( int index ) const;
	const char *GetCounterName( int index ) const;
	const char *GetCounterNameAndValue( int index, uint64 &val ) const;
	CounterGroup_t GetCounterGroup( int index ) const;

protected:
#ifdef VPROF_VTUNE_GROUP
	bool VTuneGroupEnabled() { return m_bVTuneGroupEnabled; }
	int VTuneGroupID() { return m_nVTuneGroupID; }
#endif

	static void EnterNodeScopeBackground( CVProfNode *pNode );
	// Returns true when the node's outermost scope exited
	static bool ExitNodeScopeBackground( CVProfNode *pNode );
	static void MergeBackgroundNodes_R( CVProfNode *pSource, CVProfNode *pDest );

	void FreeNodes_R( CVProfNode *pNode );

	void SumTimes( const char *pszStartNode, int budgetGroupID, CVProfSummingContext & );
	void SumTimes( const CVProfNode *pNode, int budgetGroupID, CVProfSummingContext & );
	void DumpNodes( const VProfReportSettings_t &, const CVProfNode *pNode, int indent, bool bAverageAndCountOnly, const CVProfSummingContext &, double, double, bool );
	static int FindBudgetGroupName( const char *pBudgetGroupName );
	static int AddBudgetGroupName( const char *pBudgetGroupName, int budgetFlags );

#ifdef VPROF_VTUNE_GROUP
	bool		m_bVTuneGroupEnabled;
	int			m_nVTuneGroupID;
	int			m_GroupIDStack[MAX_GROUP_STACK_DEPTH];
	int			m_GroupIDStackDepth;
#endif
	int 		m_enabled;
	CVProfNode	m_Root;
	// AMNOTE: Points at m_Root after construction
	CVProfNode *m_unk101;
	CVProfNode *m_pCurNode;
	bool		m_fAtRoot; // tracked for efficiency of the "not profiling" case
	// AMNOTE: The engine never reads or writes it
	int			m_unk201;
	int			m_nFrames;
	int			m_pausedEnabledDepth;

	// Wall clock seconds
	double		m_flStartTime;
	double		m_flStopTime;
	double		m_flPauseStartTime;
	double		m_flTotalPauseTime;

	uint64 m_Counters[MAXCOUNTERS];
	char m_CounterGroups[MAXCOUNTERS]; // (These are CounterGroup_t's).
	tchar *m_CounterNames[MAXCOUNTERS];
	int m_NumCounters;
	CAtomicMutex m_CounterMutex;

	// AMNOTE: The element types are unknown, the constructor never sets a size in them, so they may not be CUtlVectors
	CUtlVector< uint8 > m_unk301[256];
	int m_unk302;
	CUtlVector< uint8 > m_unk303[256];
	int m_unk304;
	CUtlVector< uint8 > m_unk305[256];
	int m_unk306;

	int m_nNextTimespanId;
	ThreadId_t m_TargetThreadId;
	bool m_bProfileAllThreads;
	CVProfNode *m_pBackgroundRoot;
	// Each element points at the thread-local root node pointer of a thread that profiled in the background
	CUtlVector< CVProfNode ** > m_BackgroundThreadRoots;
	CAtomicMutex m_BackgroundRootMutex;
	void (*m_pOutputStream)( const char *, ... );
};

//-------------------------------------

PLATFORM_INTERFACE CVProfile g_VProfCurrentProfile;

inline double CVProfile::GetTotalWallClockSecondsSampled()
{
	double flEndTime = m_enabled ? Plat_FloatTime() : ( m_flPauseStartTime != 0.0 ? m_flPauseStartTime : m_flStopTime );
	return flEndTime - m_flStartTime - m_flTotalPauseTime;
}

inline void CVProfile::ResetPeaks()
{
	m_Root.ResetPeak();

	AUTO_LOCK( m_BackgroundRootMutex );
	if ( m_pBackgroundRoot )
		m_pBackgroundRoot->ResetPeak();
}

// g_VProfCurrentProfile's
PLATFORM_INTERFACE int VProf_AssignNextTimespanId();
PLATFORM_INTERFACE uint64 *VProf_FindOrCreateCounter( const char *pName, CounterGroup_t eCounterGroup );


//-----------------------------------------------------------------------------

PLATFORM_INTERFACE bool g_VProfSignalSpike;

class CVProfSpikeDetector
{
public:
	CVProfSpikeDetector( float spike ) :
		m_timeLast( GetTimeLast() )
	{
		m_spike = spike;
		m_Timer.Start();
	}

	~CVProfSpikeDetector()
	{
		m_Timer.End();
		if ( Plat_FloatTime() - m_timeLast > 2.0 )
		{
			m_timeLast = Plat_FloatTime();
			if ( m_Timer.GetDuration().GetMillisecondsF() > m_spike )
			{
				g_VProfSignalSpike = true;
			}
		}
	}

private:
	static float &GetTimeLast() { static float timeLast = 0; return timeLast; }
	CFastTimer	m_Timer;
	float m_spike;
	float &m_timeLast;
};


// Macro to signal a local spike. Meant as temporary instrumentation, do not leave in code
#define VPROF_TEST_SPIKE( msec ) CVProfSpikeDetector UNIQUE_ID( msec )

//-----------------------------------------------------------------------------

struct VProfBudgetGroupCallSite
{
	VProfBudgetGroupCallSite( const char *group_name, int flags, int group_id = VPROF_BUDGET_GROUP_ID_UNACCOUNTED ) :
		m_pGroupName( group_name ), m_nFlags( flags ), m_nGroupId( group_id ) { }

	const char *m_pGroupName;
	int m_nFlags;
	int m_nGroupId;
};

template <int DETAIL_LEVEL, bool ASSERT_ACCOUNTED>
class VProfScopeHelper
{
public:
	VProfScopeHelper( const char *pszName, const CUtlSourceLocation &location )
	{
		m_pExitScope = EnterScopeInternal( pszName, location );
	}

	VProfScopeHelper( const char *pszName, const char *pszGroupName, int nFlags, const CUtlSourceLocation &location )
	{
		VProfBudgetGroupCallSite budget_group( pszGroupName, nFlags );
		m_pExitScope = EnterScopeInternalBudgetFlags( pszName, budget_group, location );
	}

	VProfScopeHelper( VProfScopeHelper &other )
	{
		m_pExitScope = other.m_pExitScope;
	}

	~VProfScopeHelper()
	{
		ExitScope();
	}

	void ExitScope()
	{
		if(!m_pExitScope)
			return;

		m_pExitScope();
		m_pExitScope = nullptr;
	}

	VProfScopeHelper &operator=( const VProfScopeHelper &other ) = delete;
	VProfScopeHelper &operator=( VProfScopeHelper &&other ) = delete;

	static DLL_CLASS_IMPORT VProfExitScopeCB EnterScopeInternal( const char *pszName, const CUtlSourceLocation &location );
	static DLL_CLASS_IMPORT VProfExitScopeCB EnterScopeInternalBudgetFlags( const char *pszName, VProfBudgetGroupCallSite &budgetGroup, const CUtlSourceLocation &location );

private:
	VProfExitScopeCB m_pExitScope = nullptr;
};

//-----------------------------------------------------------------------------

class CVProfCounter
{
public:
	CVProfCounter( const char *pName, CounterGroup_t group = COUNTER_GROUP_DEFAULT )
	{
		m_pCounter = g_VProfCurrentProfile.FindOrCreateCounter( pName, group );
		Assert( m_pCounter );
	}
	~CVProfCounter()
	{
	}
	void Increment( uint64 val )
	{ 
		Assert( m_pCounter );
		*m_pCounter += val; 
	}
private:
	uint64 *m_pCounter;
};

#endif

#define VPROF_BEGIN_PIX_BLOCK( PIXConvar ) {
#define VPROF_END_PIX_BLOCK() }


#ifdef VPROF_UNDO_PIX
#undef USE_PIX
#undef _PIX_H_
#undef PIXBeginNamedEvent
#undef PIXEndNamedEvent
#undef PIXSetMarker
#undef PIXNameThread
#include <pix.h>
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif

//=============================================================================
