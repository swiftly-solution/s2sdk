#ifndef LOGGINGTYPES_H
#define LOGGINGTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "platform.h"
#include "color.h"
#include <stdarg.h>

//-----------------------------------------------------------------------------
// Maximum length of a sprintf'ed logging message.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_MESSAGE_LENGTH = 2048;

//-----------------------------------------------------------------------------
// Maximum length of a channel or tag name.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_IDENTIFIER_LENGTH = 32;

//-----------------------------------------------------------------------------
// Maximum number of logging channels.  Increase if needed.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_CHANNEL_COUNT = 256;

//-----------------------------------------------------------------------------
// Maximum number of logging tags across all channels.  Increase if needed.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_TAG_COUNT = 1024;

//-----------------------------------------------------------------------------
// Maximum number of characters across all logging tags. Increase if needed.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_TAG_CHARACTER_COUNT = 8192;

//-----------------------------------------------------------------------------
// Maximum number of concurrent logging listeners in a given logging state.
//-----------------------------------------------------------------------------
const int MAX_LOGGING_LISTENER_COUNT = 16;

//-----------------------------------------------------------------------------
// An invalid color set on a channel to imply that it should use
// a device-dependent default color where applicable.
//-----------------------------------------------------------------------------
const Color UNSPECIFIED_LOGGING_COLOR( 0, 0, 0, 0 );

//-----------------------------------------------------------------------------
// An ID returned by the logging system to refer to a logging channel.
//-----------------------------------------------------------------------------
typedef int LoggingChannelID_t;

//-----------------------------------------------------------------------------
// A sentinel value indicating an invalid logging channel ID.
//-----------------------------------------------------------------------------
const LoggingChannelID_t INVALID_LOGGING_CHANNEL_ID = -1;

//-----------------------------------------------------------------------------
// An handle returned by the logging system to refer to a tag.
//-----------------------------------------------------------------------------
typedef int LoggingTagHandle_t;

//-----------------------------------------------------------------------------
// The severity of a logging operation.
//-----------------------------------------------------------------------------
enum LoggingSeverity_t
{
	//-----------------------------------------------------------------------------
	// A debug message, more verbose than LS_DETAILED.
	//-----------------------------------------------------------------------------
	LS_MORE_DETAILED = 0,

	//-----------------------------------------------------------------------------
	// A debug message.
	//-----------------------------------------------------------------------------
	LS_DETAILED = 1,

	//-----------------------------------------------------------------------------
	// An informative logging message.
	//-----------------------------------------------------------------------------
	LS_MESSAGE = 2,

	//-----------------------------------------------------------------------------
	// A warning, typically non-fatal
	//-----------------------------------------------------------------------------
	LS_WARNING = 3,

	//-----------------------------------------------------------------------------
	// A message caused by an Assert**() operation.
	//-----------------------------------------------------------------------------
	LS_ASSERT = 4,

	//-----------------------------------------------------------------------------
	// An error, typically fatal/unrecoverable.
	//-----------------------------------------------------------------------------
	LS_ERROR = 5,

	//-----------------------------------------------------------------------------
	// A placeholder level, higher than any legal value.
	// Not a real severity value!
	//-----------------------------------------------------------------------------
	LS_HIGHEST_SEVERITY = 6,
};

enum LoggingVerbosity_t
{
	//-----------------------------------------------------------------------------
	// Turns off all spew.
	//-----------------------------------------------------------------------------
	LV_OFF = 0,

	//-----------------------------------------------------------------------------
	// Turns on vital logs.
	//-----------------------------------------------------------------------------
	LV_ESSENTIAL = 1,

	//-----------------------------------------------------------------------------
	// Turns on most messages.
	//-----------------------------------------------------------------------------
	LV_DEFAULT = 2,

	//-----------------------------------------------------------------------------
	// Allows for walls of text that are usually useful.
	//-----------------------------------------------------------------------------
	LV_DETAILED = 3,

	//-----------------------------------------------------------------------------
	// Allows everything.
	//-----------------------------------------------------------------------------
	LV_MAX = 4,
};

//-----------------------------------------------------------------------------
// Action which should be taken by logging system as a result of 
// a given logged message.
//
// The logging system invokes ILoggingResponsePolicy::OnLog() on
// the specified policy object, which returns a LoggingResponse_t.
//-----------------------------------------------------------------------------
enum LoggingResponse_t
{
	LR_CONTINUE,
	LR_DEBUGGER,
	LR_ABORT,
};

//-----------------------------------------------------------------------------
// Logging channel behavior flags, set on channel creation.
//-----------------------------------------------------------------------------
enum LoggingChannelFlags_t
{
	//-----------------------------------------------------------------------------
	// Indicates that the spew is only relevant to interactive consoles.
	//-----------------------------------------------------------------------------
	LCF_CONSOLE_ONLY = 0x00000001,

	//-----------------------------------------------------------------------------
	// Indicates that spew should not be echoed to any output devices.
	// A suitable logging listener must be registered which respects this flag 
	// (e.g. a file logger).
	//-----------------------------------------------------------------------------
	LCF_DO_NOT_ECHO = 0x00000002,
};

//-----------------------------------------------------------------------------
// A callback function used to register tags on a logging channel 
// during initialization.
//-----------------------------------------------------------------------------
typedef void ( *RegisterTagsFunc )( LoggingChannelID_t channelID );

struct LoggingMetaData_t
{
	LoggingMetaData_t *m_pNext;
	uint64_t m_TypeID;
	uint8_t *m_pData;
	size_t m_nDataSize;
	uint8_t m_nVerbosity;
};

struct LoggingRareOptions_t
{
	const char *m_File = nullptr;
	int m_Line = -1;
	const char *m_Function = nullptr;
	const LoggingMetaData_t *m_pMetaData = nullptr;
	// Replaces the channel color
	const Color *m_pColor = nullptr;
	// Used instead of the arguments of the call
	va_list *m_pVAListOverride = nullptr;
	// AMNOTE: Passed on to listeners after LoggingContext_t::m_MetaData, asserts point it at a 1088-byte buffer starting with a zero int
	void *m_unk101 = nullptr;
	// AMNOTE: Passed on to listeners after the pointer, VConsole lets a message over 8 MB reach a local client when bit 0 is set
	int m_unk102 = 0;
};

//-----------------------------------------------------------------------------
// A context structure passed to logging listeners and response policy classes.
//-----------------------------------------------------------------------------
struct LoggingContext_t
{
	// ID of the channel being logged to.
	LoggingChannelID_t m_ChannelID;
	// Flags associated with the channel.
	LoggingChannelFlags_t m_Flags;
	// Severity of the logging event.
	LoggingSeverity_t m_Severity;
	// Color of logging message if one was specified to Log_****() macro.
	// If not specified, falls back to channel color.
	// If channel color is not specified, this value is UNSPECIFIED_LOGGING_COLOR
	// and indicates that a suitable default should be chosen.
	Color m_Color;
	const LoggingMetaData_t *m_MetaData;
};

//-----------------------------------------------------------------------------
// Interface for classes to handle logging output.
//
// The Log() function of this class is called synchronously and serially
// by the logging system on all registered instances of ILoggingListener
// in the current "logging state".
//
// Derived classes may do whatever they want with the message (write to disk,
// write to console, send over the network, drop on the floor, etc.).
//
// In general, derived classes should do one, simple thing with the output
// to allow callers to register multiple, orthogonal logging listener classes.
//-----------------------------------------------------------------------------
class ILoggingListener
{
public:
	virtual void Log( const LoggingContext_t *pContext, const char *pMessage ) = 0;
	virtual void OnFlush();
	virtual void OnChannelRegistered( LoggingChannelID_t channelID );
	virtual void OnChannelVerbosityChanged( LoggingChannelID_t channelID );
	virtual void OnChannelFlagsChanged( LoggingChannelID_t channelID );
};

//-----------------------------------------------------------------------------
// Interface for policy classes which determine how to behave when a 
// message is logged.
//
// Can return:
//   LR_CONTINUE (continue execution)
//   LR_DEBUGGER (break into debugger if one is present, otherwise continue)
//   LR_ABORT (terminate process immediately with a failure code of 1)
//-----------------------------------------------------------------------------
class ILoggingResponsePolicy
{
public:
	virtual LoggingResponse_t OnLog( const LoggingContext_t *pContext ) = 0;
};

#endif