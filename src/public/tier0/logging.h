//============ Copyright (c) Valve Corporation, All rights reserved. ============
//
// Logging system declarations.
//
// The logging system is a channel-based output mechanism which allows
// subsystems to route their text/diagnostic output to various listeners
//
//===============================================================================

#ifndef LOGGING_H
#define LOGGING_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

#include "color.h"
#include "loggingtypes.h"
#include "logginglisteners.h"
#include "loggingresponsepolicies.h"
#include "icommandline.h"
#include <stdio.h>

/*
	---- Logging System ----

	The logging system is a channel-based mechanism for all code (engine,
	mod, tool) across all platforms to output information, warnings, 
	errors, etc.

	This system supersedes the existing Msg(), Warning(), Error(), DevMsg(), ConMsg() etc. functions.
	There are channels defined in the new system through which all old messages are routed;
	see LOG_GENERAL, LOG_CONSOLE, LOG_DEVELOPER, etc.

	To use the system, simply call one of the predefined macros:

		Log_Msg( ChannelID, [Color], Message, ... )
		Log_Warning( ChannelID, [Color], Message, ... )
		Log_Error( ChannelID, [Color], Message, ... )

	A ChannelID is typically created by defining a logging channel with the
	log channel macros:

		DEFINE_LOGGING_CHANNEL_NO_TAGS( LOG_ChannelName, "ChannelName", [Flags], [Verbosity], [Color] );
	
		or

		BEGIN_DEFINE_LOGGING_CHANNEL( LOG_ChannelName, "ChannelName", [Flags], [Verbosity], [Color] );
		ADD_LOGGING_CHANNEL_TAG( "Tag1" );
		ADD_LOGGING_CHANNEL_TAG( "Tag2" );
		END_DEFINE_LOGGING_CHANNEL();

	These macros create a global channel ID variable with the name specified 
	by the first parameter (in this example, LOG_ChannelName).  This channel ID 
	can be used by various LoggingSystem_** functions to manipulate the channel settings.

	The optional [Flags] parameter is an OR'd together set of LoggingChannelFlags_t 
	values (default: 0).

	The optional [Verbosity] parameter is the highest threshold
	below which messages will be processed (inclusive).  The default is LV_DEFAULT,
	which results in all messages, warnings, and errors being logged. 
	Variadic parameters to the Log_** functions will be ignored if a channel
	is not enabled for a given verbosity (for performance reasons).

	Logging channels can have their verbosity modified by name, ID, or tag.

	Logging channels are not hierarchical since there are situations in which
	a channel needs to belong to multiple hierarchies.  Use tags to create
	categories or shallow hierarchies.

	@TODO (Feature wishlist):
	1) Callstack logging support
	2) Registering dynamic channels and unregistering channels at runtime
	3) Sentient robot to clean up the thousands of places using the old/legacy logging system.
*/

//////////////////////////////////////////////////////////////////////////
// Constants, Types, Forward Declares
//////////////////////////////////////////////////////////////////////////

class CLoggingSystem;
class CAtomicMutex;
class CMiniDumpComment;
class CUtlString;
template< class T, class I, size_t UNK > class CUtlVectorMemory_Growable;
template< class T, class I, class A > class CUtlVector;

enum ModificationFlags_t : uint8
{
	MODIFIED_FLAGS = 1,
	MODIFIED_VERBOSITY = 2,
	MODIFIED_SPEWCOLOR = 4,
};

//////////////////////////////////////////////////////////////////////////
// Central Logging System
//////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// The central logging system.
//
// Multiple instances can exist, though all exported tier0 functionality
// specifically works with a single global instance 
// (via GetGlobalLoggingSystem()).
//-----------------------------------------------------------------------------
class CLoggingSystem
{
public:
	struct LoggingChannel_t;

	CLoggingSystem();
	~CLoggingSystem();

	//-----------------------------------------------------------------------------
	// Register a logging channel with the logging system.
	// The same channel can be registered multiple times, but the parameters
	// in each call to RegisterLoggingChannel must either match across all calls
	// or be set to defaults on any given call
	//
	// This function is not thread-safe and should generally only be called 
	// by a single thread. Using the logging channel definition macros ensures 
	// that this is called on the static initialization thread.
	//-----------------------------------------------------------------------------
	LoggingChannelID_t RegisterLoggingChannel( const char *pChannelName, RegisterTagsFunc registerTagsFunc, int flags = 0, LoggingVerbosity_t verbosity = LV_DEFAULT, Color spewColor = UNSPECIFIED_LOGGING_COLOR );

	//-----------------------------------------------------------------------------
	// Gets a channel ID from a string name.
	// Performs a simple linear search; cache the value whenever possible
	// or re-register the logging channel to get a global ID.
	//-----------------------------------------------------------------------------
	LoggingChannelID_t FindChannel( const char *pChannelName ) const;

	int GetChannelCount() const;

	//-----------------------------------------------------------------------------
	// Gets a pointer to the logging channel description.
	//-----------------------------------------------------------------------------
	LoggingChannel_t *GetChannel( LoggingChannelID_t channelID );
	const LoggingChannel_t *GetChannel( LoggingChannelID_t channelID ) const;
	
	//-----------------------------------------------------------------------------
	// Returns true if the given channel has the specified tag.
	//-----------------------------------------------------------------------------
	bool HasTag( LoggingChannelID_t channelID, const char *pTag ) const;
	
	//-----------------------------------------------------------------------------
	// Returns true if the given channel will spew at the given verbosity level.
	//-----------------------------------------------------------------------------
	bool IsChannelEnabled( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity ) const;

	//-----------------------------------------------------------------------------
	// Functions to set the verbosity level of a channel either directly by ID or 
	// string name, or for all channels with a given tag.
	//
	// These functions are not technically thread-safe but calling them across 
	// multiple threads should cause no significant problems 
	// (the underlying data types being changed are 32-bit/atomic).
	//-----------------------------------------------------------------------------
	void SetChannelVerbosity( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
	void SetChannelVerbosityByName( const char *pName, LoggingVerbosity_t verbosity );
	void SetChannelVerbosityByTag( const char *pTag, LoggingVerbosity_t verbosity );

	//-----------------------------------------------------------------------------
	// Gets or sets the color of a logging channel.
	// (The functions are not thread-safe, but the consequences are not 
	// significant.)
	//-----------------------------------------------------------------------------
	Color GetChannelColor( LoggingChannelID_t channelID ) const;
	void SetChannelColor( LoggingChannelID_t channelID, Color color );

	//-----------------------------------------------------------------------------
	// Gets or sets the flags on a logging channel.
	// (The functions are not thread-safe, but the consequences are not 
	// significant.)
	//-----------------------------------------------------------------------------
	LoggingChannelFlags_t GetChannelFlags( LoggingChannelID_t channelID ) const;
	void SetChannelFlags( LoggingChannelID_t channelID, LoggingChannelFlags_t flags );

	//-----------------------------------------------------------------------------
	// Adds a string tag to a channel.
	// This is not thread-safe and should only be called by a RegisterTagsFunc
	// callback passed in to RegisterLoggingChannel (via the 
	// channel definition macros).
	//-----------------------------------------------------------------------------
	void AddTagToChannel( LoggingChannelID_t channelID, const char *pTagName );

	//-----------------------------------------------------------------------------
	// Functions to save/restore the current logging state.  
	// Set bThreadLocal to true on a matching Push/Pop call if the intent
	// is to override the logging listeners on the current thread only.
	//
	// Pushing the current logging state onto the state stack results
	// in the current state being cleared by default (no listeners, default logging response policy).
	// Set bClearState to false to copy the existing listener pointers to the new state.
	//
	// These functions which mutate logging state ARE thread-safe and are 
	// guarded by m_StateMutex.
	//-----------------------------------------------------------------------------
	void PushLoggingState( bool bThreadLocal = false, bool bClearState = true );
	void PopLoggingState( bool bThreadLocal = false );

	//-----------------------------------------------------------------------------
	// Registers a logging listener (a class which handles logged messages).
	//-----------------------------------------------------------------------------
	void RegisterLoggingListener( ILoggingListener *pListener );
	void RegisterBackdoorLoggingListener( ILoggingListener *pListener );

	//-----------------------------------------------------------------------------
	// Unregisters a logging listener.
	//-----------------------------------------------------------------------------
	void UnregisterLoggingListener( ILoggingListener *pListener );

	//-----------------------------------------------------------------------------
	// Enable or disable backdoor logging listeners.
	//-----------------------------------------------------------------------------	
	void EnableBackdoorLoggingListeners( bool bEnable );

	//-----------------------------------------------------------------------------
	// Returns whether the specified logging listener is registered.
	//-----------------------------------------------------------------------------
	bool IsListenerRegistered( ILoggingListener *pListener ) const;

	//-----------------------------------------------------------------------------
	// Clears out all of the current logging state (removes all listeners, 
	// sets the response policy to the default).
	//-----------------------------------------------------------------------------
	void ResetCurrentLoggingState();
	void ResetBackdoorLoggingState();

	//-----------------------------------------------------------------------------
	// Sets a policy class to decide what should happen when messages of a 
	// particular severity are logged 
	// (e.g. exit on error, break into debugger).
	// If pLoggingResponse is NULL, uses the default response policy class.
	//-----------------------------------------------------------------------------
	void SetLoggingResponsePolicy( ILoggingResponsePolicy *pLoggingResponse );
	
	//-----------------------------------------------------------------------------
	// Logs a message to the specified channel using a given severity and 
	// spew color.  Passing in UNSPECIFIED_LOGGING_COLOR for 'color' allows
	// the logging listeners to provide a default.
	//-----------------------------------------------------------------------------
	LoggingResponse_t LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t *pRareOptions, const LoggingMetaData_t *metaData, Color color, const tchar *pMessage );
	
	// Internal data to represent a logging tag
	struct LoggingTag_t
	{
		const char *m_pTagName;
		LoggingTag_t *m_pNextTag;
	};

	// Internal data to represent a logging channel.
	struct LoggingChannel_t
	{	
		bool HasTag( const char *pTag ) const;
		bool IsEnabled( LoggingVerbosity_t verbosity ) const;
		// It is recommended to use LoggingSystem_SetChannelVerbosity instead, as SetVerbosity bypasses ILoggingListener::OnChannelVerbosityChanged callbacks.
		void SetVerbosity( LoggingVerbosity_t verbosity );

		LoggingChannelID_t m_ID;
		LoggingChannelFlags_t m_DefaultFlags;	
		LoggingChannelFlags_t m_Flags;
		LoggingVerbosity_t m_DefaultVerbosity;	
		LoggingVerbosity_t m_Verbosity;
		int m_nIndent;
		Color m_SpewColor;
		char m_Name[MAX_LOGGING_IDENTIFIER_LENGTH];
		LoggingTag_t *m_pFirstTag;
		ModificationFlags_t m_nModifiedFields;
		// Whether the last message logged to the channel ended with a newline
		bool m_bLastLineComplete;
	};

private:
	// Represents the current state of the logger (registered listeners, response policy class, etc.) and can
	// vary from thread-to-thread.  It can also be pushed/popped to save/restore listener/response state.
	struct LoggingState_t
	{
		// Number of active listeners in this set.  Cannot exceed MAX_LOGGING_LISTENER_COUNT.
		// If set to -1, implies that this state structure is not in use.
		int m_nListenerCount;

		// Array of registered logging listener objects.
		ILoggingListener *m_RegisteredListeners[MAX_LOGGING_LISTENER_COUNT];

		// Index of the previous entry on the listener set stack.
		int m_nPreviousStackEntry;

		// Specific policy class to determine behavior of logging system under specific message types.
		ILoggingResponsePolicy *m_pLoggingResponse;
	};

	struct BackdoorLoggingState_t
	{
		// Number of active listeners in this set.  Cannot exceed MAX_LOGGING_LISTENER_COUNT.
		// If set to -1, implies that this state structure is not in use.
		int m_nListenerCount;

		// Array of registered logging listener objects.
		ILoggingListener *m_RegisteredListeners[MAX_LOGGING_LISTENER_COUNT];

		// Are the backdoor logging listeners enabled?
		bool m_bEnabled;
	};

	// These state functions to assume the caller has already grabbed the mutex.
	LoggingState_t *GetCurrentState();
	const LoggingState_t *GetCurrentState() const;

	int FindUnusedStateIndex();
	LoggingTag_t *AllocTag( const char *pTagName );

	int m_nChannelCount;
	LoggingChannel_t m_RegisteredChannels[MAX_LOGGING_CHANNEL_COUNT];

	int m_nChannelTagCount;
	LoggingTag_t m_ChannelTags[MAX_LOGGING_TAG_COUNT];
	
	// Index to first free character in name pool.
	int m_nTagNamePoolIndex;
	// Pool of character data used for tag names.
	char m_TagNamePool[MAX_LOGGING_TAG_CHARACTER_COUNT];

	// Protects all data in this class except the registered channels 
	// (which are supposed to be registered using the macros at static/global init time).
	// It is assumed that this mutex is reentrant safe on all platforms.
	CAtomicMutex *m_pStateMutex;
	
	// The index of the current "global" state of the logging system.  By default, all threads use this state
	// for logging unless a given thread has pushed the logging state with bThreadLocal == true.
	// If a thread-local state has been pushed, g_nThreadLocalStateIndex (a global thread-local integer) will be non-zero.
	// By default, g_nThreadLocalStateIndex is 0 for all threads.
	int m_nGlobalStateIndex;
	
	// A pool of logging states used to store a stack (potentially per-thread).
	static const int MAX_LOGGING_STATE_COUNT = 64;
	LoggingState_t m_LoggingStates[MAX_LOGGING_STATE_COUNT];

	// Backdoor logging state
	BackdoorLoggingState_t m_BackdoorLoggingState;

	// Default policy class which determines behavior.
	CDefaultLoggingResponsePolicy m_DefaultLoggingResponse;

	// Default spew function.
	CColorizedLoggingListener m_DefaultLoggingListener;
	
};

//////////////////////////////////////////////////////////////////////////
// Logging Macros
//////////////////////////////////////////////////////////////////////////

// This macro will resolve to the most appropriate overload of LoggingSystem_Log() depending on the number of parameters passed in.
#define InternalMsg( Channel, Severity, /* [Color], Message, */ ... ) do { if ( LoggingSystem_IsChannelEnabled( Channel, Severity ) ) LoggingSystem_Log( Channel, Severity, /* [Color], Message, */ ##__VA_ARGS__ ); } while( 0 )

//-----------------------------------------------------------------------------
// New macros, use these!
//
// The macros take an optional Color parameter followed by the message 
// and the message formatting.
// We rely on the variadic macro (__VA_ARGS__) operator to paste in the 
// extra parameters and resolve to the appropriate overload.
//-----------------------------------------------------------------------------
#define Log_Msg( Channel, /* [Color], Message, */ ... ) InternalMsg( Channel, LS_MESSAGE, /* [Color], Message, */ ##__VA_ARGS__ )
#define Log_Warning( Channel, /* [Color], Message, */ ... ) InternalMsg( Channel, LS_WARNING, /* [Color], Message, */ ##__VA_ARGS__ )
#define Log_Error( Channel, /* [Color], Message, */ ... ) InternalMsg( Channel, LS_ERROR, /* [Color], Message, */ ##__VA_ARGS__ )
#define Log_Assert( Message, ... ) LoggingSystem_LogAssert( Message, ##__VA_ARGS__ )


#define DECLARE_LOGGING_CHANNEL( Channel ) extern LoggingChannelID_t Channel

#define DEFINE_LOGGING_CHANNEL_NO_TAGS( Channel, ChannelName, /* [Flags], [Verbosity], [Color] */ ... ) \
	LoggingChannelID_t Channel = LoggingSystem_RegisterLoggingChannel( ChannelName, NULL, ##__VA_ARGS__ )

#define BEGIN_DEFINE_LOGGING_CHANNEL( Channel, ChannelName, /* [Flags], [Verbosity], [Color] */ ... ) \
	static void Register_##Channel##_Tags( LoggingChannelID_t channelID ); \
	LoggingChannelID_t Channel = LoggingSystem_RegisterLoggingChannel( ChannelName, Register_##Channel##_Tags, ##__VA_ARGS__ ); \
	void Register_##Channel##_Tags( LoggingChannelID_t channelID ) \
	{

#define ADD_LOGGING_CHANNEL_TAG( Tag ) LoggingSystem_AddTagToChannel( channelID, Tag )

#define END_DEFINE_LOGGING_CHANNEL() \
	}
	
//////////////////////////////////////////////////////////////////////////
// DLL Exports
//////////////////////////////////////////////////////////////////////////

// For documentation on these functions, please look at the corresponding function
// in CLoggingSystem (unless otherwise specified).
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_RegisterLoggingChannel( const char *pName, RegisterTagsFunc registerTagsFunc, int flags = 0, LoggingVerbosity_t verbosity = LV_DEFAULT, Color color = UNSPECIFIED_LOGGING_COLOR ); 

PLATFORM_INTERFACE void LoggingSystem_ResetCurrentLoggingState();
PLATFORM_INTERFACE void LoggingSystem_ResetBackdoorLoggingState();

PLATFORM_INTERFACE void LoggingSystem_RegisterLoggingListener( ILoggingListener *pListener );
PLATFORM_INTERFACE void LoggingSystem_RegisterBackdoorLoggingListener( ILoggingListener *pListener );
PLATFORM_INTERFACE void LoggingSystem_EnableBackdoorLoggingListeners( bool bEnable );

PLATFORM_INTERFACE void LoggingSystem_UnregisterLoggingListener( ILoggingListener *pListener );

PLATFORM_INTERFACE void LoggingSystem_SetLoggingResponsePolicy( ILoggingResponsePolicy *pResponsePolicy );

// NOTE: PushLoggingState() saves the current logging state on a stack and results in a new clear state
// (no listeners, default logging response policy).
PLATFORM_INTERFACE void LoggingSystem_PushLoggingState( bool bThreadLocal = false, bool bClearState = true );
PLATFORM_INTERFACE void LoggingSystem_PopLoggingState( bool bThreadLocal = false );
PLATFORM_INTERFACE int LoggingSystem_CurrentStateIndex( bool bThreadLocal = false );

PLATFORM_INTERFACE void LoggingSystem_AddTagToChannel( LoggingChannelID_t channelID, const char *pTagName );

// Returns INVALID_LOGGING_CHANNEL_ID if not found
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_FindChannel( const char *pChannelName );
PLATFORM_INTERFACE int LoggingSystem_GetChannelCount();
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_GetFirstChannelID();
// Returns INVALID_LOGGING_CHANNEL_ID when there are no channels remaining.
PLATFORM_INTERFACE LoggingChannelID_t LoggingSystem_GetNextChannelID( LoggingChannelID_t channelID );
PLATFORM_INTERFACE const CLoggingSystem::LoggingChannel_t *LoggingSystem_GetChannel( LoggingChannelID_t channelID );

PLATFORM_INTERFACE bool LoggingSystem_HasTag( LoggingChannelID_t channelID, const char *pTag );

PLATFORM_INTERFACE bool LoggingSystem_IsChannelEnabled( LoggingChannelID_t channelID, LoggingSeverity_t severity );
PLATFORM_OVERLOAD bool LoggingSystem_IsChannelEnabled( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
PLATFORM_INTERFACE LoggingVerbosity_t LoggingSystem_GetChannelVerbosity( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetChannelVerbosity( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
PLATFORM_INTERFACE void LoggingSystem_SetChannelVerbosityByName( const char *pName, LoggingVerbosity_t verbosity );
PLATFORM_INTERFACE void LoggingSystem_SetChannelVerbosityByTag( const char *pTag, LoggingVerbosity_t verbosity );
// Raises the channel's verbosity to the given one if it's lower
PLATFORM_INTERFACE void LoggingSystem_EnsureChannelVerbosity( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity );
// The lowest verbosity a channel needs to log messages of the given severity
PLATFORM_INTERFACE LoggingVerbosity_t LoggingSystem_GetVerbosityForSeverity( LoggingSeverity_t severity );

// Color is represented as an int32 due to C-linkage restrictions
PLATFORM_INTERFACE int32 LoggingSystem_GetChannelColor( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetChannelColor( LoggingChannelID_t channelID, int color );

PLATFORM_INTERFACE LoggingChannelFlags_t LoggingSystem_GetChannelFlags( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetChannelFlags( LoggingChannelID_t channelID, LoggingChannelFlags_t flags );

// Return the channel's previous indent
PLATFORM_INTERFACE int LoggingSystem_SetChannelIndent( LoggingChannelID_t channelID, int nIndent );
PLATFORM_INTERFACE int LoggingSystem_ChangeChannelIndent( LoggingChannelID_t channelID, int nDelta );

// Whether the last message logged to the channel ended with a newline
PLATFORM_INTERFACE bool LoggingSystem_GetLastChannelLineComplete( LoggingChannelID_t channelID );
PLATFORM_INTERFACE void LoggingSystem_SetLastChannelLineComplete( LoggingChannelID_t channelID, bool bComplete );

// Calls ILoggingListener::OnFlush on the current and backdoor listeners
PLATFORM_INTERFACE void LoggingSystem_Flush();

//-----------------------------------------------------------------------------
// Logs a variable-argument to a given channel with the specified severity.
// NOTE: if adding overloads to this function, remember that the Log_***
// macros simply pass their variadic parameters through to LoggingSystem_Log().
// Therefore, you need to ensure that the parameters are in the same general 
// order and that there are no ambiguities with the overload.
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessageFormat, ... ) FMTFUNCTION( 3, 4 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingMetaData_t *metaData, const char *pMessageFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingMetaData_t *metaData, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 5, 6 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, const char *pMessageFormat, ... ) FMTFUNCTION( 4, 5 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 5, 6 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, const LoggingMetaData_t *metaData, const char *pMessageFormat, ... ) FMTFUNCTION( 5, 6 );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_Log( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, const LoggingMetaData_t *metaData, Color spewColor, const char *pMessageFormat, ... ) FMTFUNCTION( 6, 7 );

PLATFORM_INTERFACE LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingMetaData_t *metaData, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, Color spewColor, const LoggingMetaData_t *metaData, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, Color spewColor, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, const LoggingMetaData_t *metaData, const char *pMessage );
PLATFORM_OVERLOAD LoggingResponse_t LoggingSystem_LogDirect( LoggingChannelID_t channelID, LoggingSeverity_t severity, const LoggingRareOptions_t &rareOptions, Color spewColor, const LoggingMetaData_t *metaData, const char *pMessage );

//-----------------------------------------------------------------------------
// Logs pText one line at a time, each with LoggingSystem_LogDirect.
// nTextLength is the length of pText, or -1 when it's null terminated.
// nMaxLines limits how many lines are logged, 0 for no limit: a positive value keeps the first lines
// and a negative one the last ones, logging how many were omitted instead of the rest.
// Returns the number of messages logged.
//-----------------------------------------------------------------------------
PLATFORM_INTERFACE int LoggingSystem_LogLines( LoggingChannelID_t channelID, LoggingSeverity_t severity, const char *pText, int nTextLength = -1, int nMaxLines = 0 );

PLATFORM_INTERFACE LoggingResponse_t LoggingSystem_LogAssert( const char *pMessageFormat, ... ) FMTFUNCTION( 1, 2 );

PLATFORM_INTERFACE LoggingTagHandle_t LoggingSystem_FirstTag();
PLATFORM_INTERFACE LoggingTagHandle_t LoggingSystem_NextTag( LoggingTagHandle_t currentTag );
PLATFORM_INTERFACE LoggingTagHandle_t LoggingSystem_InvalidTag();
PLATFORM_INTERFACE const char *LoggingSystem_TagName( LoggingTagHandle_t tag );

//-----------------------------------------------------------------------------
// Log capture: keeps the latest logged lines for crash reports.
//-----------------------------------------------------------------------------
PLATFORM_OVERLOAD void LoggingSystem_GetLogCapture( CUtlVector< CUtlString, int, CUtlVectorMemory_Growable< CUtlString, int, 0 > > *pLines, bool bReversed );
PLATFORM_OVERLOAD void LoggingSystem_GetLogCapture( CMiniDumpComment *pComment, bool bReversed );
PLATFORM_OVERLOAD bool LoggingSystem_GetLogCaptureForMiniDump( CMiniDumpComment *pComment, bool bReversed, const char *pMessage );

PLATFORM_INTERFACE void LoggingSystem_AddLogCapture( const char *pFormat, ... ) FMTFUNCTION( 1, 2 );
PLATFORM_INTERFACE void LoggingSystem_AddLogCaptureString( const char *pString );
PLATFORM_INTERFACE void LoggingSystem_AddLogCaptureV( const char *pFormat, va_list args );
PLATFORM_INTERFACE void LoggingSystem_ClearLogCapture();
// Calls are counted, so capture stays enabled until it is disabled as often as enabled. Returns whether it is enabled.
PLATFORM_INTERFACE bool LoggingSystem_EnableLogCapture( bool bEnable );
PLATFORM_INTERFACE void LoggingSystem_EnableLogCaptureInMiniDumpComment( bool bEnable );
PLATFORM_INTERFACE int LoggingSystem_GetMaxLogCaptureLines();
PLATFORM_INTERFACE void LoggingSystem_SetMaxLogCaptureLines( int nLines );

#endif // LOGGING_H
