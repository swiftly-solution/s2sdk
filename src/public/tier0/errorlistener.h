#ifndef TIER0_ERRORLISTENER_H
#define TIER0_ERRORLISTENER_H

#ifdef _WIN32
#pragma once
#endif

struct MinimalBufferString
{
    int m_nLength;
	int m_nAllocatedSize;
    char *m_pString;
};

// AMNOTE: Names are guesses
enum ErrorListenerSeverity_t
{
	ERROR_LISTENER_SEVERITY_MESSAGE = 0,
	ERROR_LISTENER_SEVERITY_WARNING = 1,
	ERROR_LISTENER_SEVERITY_ERROR = 2,
};

// AMNOTE: Only the virtual destructor is known
class IBaseErrorContext
{
public:
	virtual ~IBaseErrorContext();
};

struct ErrorListenerMessage_t
{
	ErrorListenerMessage_t();
	~ErrorListenerMessage_t();

	ErrorListenerMessage_t( const ErrorListenerMessage_t & ) = delete;
	ErrorListenerMessage_t &operator=( const ErrorListenerMessage_t & ) = delete;

	MinimalBufferString m_sMessage;
	int m_nSeverity; // ErrorListenerSeverity_t
	char* m_sLocation;
	int m_nLine;
	int m_nColumn;
	IBaseErrorContext *m_pContext;
};

// AMNOTE: Name is a guess
struct ErrorContextHandle_t
{
	int m_nContextLength;
};

class IErrorListener
{
public:
	virtual void ReportMessage( const ErrorListenerMessage_t &message ) = 0;
	virtual bool FormatMessageLocation( const ErrorListenerMessage_t &message, MinimalBufferString &sOutLocation, const char *pszSuffix ) = 0;
	virtual const char *GetSourceName( int *pLineAndColumn ) = 0;
	virtual bool AppendContextPath( MinimalBufferString &sOutContextPath ) = 0;
	virtual ErrorContextHandle_t PushContext( const char *pszFormat, ... ) = 0;

	// A negative length does nothing
	virtual void PopContext( int nContextLength ) = 0;
};

#endif