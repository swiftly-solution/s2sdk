#include "public/tier0/errorlistener.h"

IBaseErrorContext::~IBaseErrorContext()
{}

ErrorListenerMessage_t::ErrorListenerMessage_t()
: m_nSeverity( ERROR_LISTENER_SEVERITY_MESSAGE ), m_nLine( 0 ), m_nColumn( 0 ), m_pContext( nullptr )
{}

ErrorListenerMessage_t::~ErrorListenerMessage_t()
{ delete m_pContext; }
