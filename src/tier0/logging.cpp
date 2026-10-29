#include "public/tier0/logging.h"

int CLoggingSystem:: GetChannelCount() const
{ return m_nChannelCount; }

CLoggingSystem::LoggingChannel_t * CLoggingSystem::GetChannel( LoggingChannelID_t channelID )
{ return &m_RegisteredChannels[channelID]; }

const CLoggingSystem::LoggingChannel_t * CLoggingSystem::GetChannel( LoggingChannelID_t channelID ) const
{ return &m_RegisteredChannels[channelID]; }

bool CLoggingSystem:: HasTag( LoggingChannelID_t channelID, const char *pTag ) const
{ return GetChannel( channelID )->HasTag( pTag ); }

bool CLoggingSystem:: IsChannelEnabled( LoggingChannelID_t channelID, LoggingVerbosity_t verbosity ) const
{ return GetChannel( channelID )->IsEnabled( verbosity ); }

Color CLoggingSystem:: GetChannelColor( LoggingChannelID_t channelID ) const
{ return GetChannel( channelID )->m_SpewColor; }

void CLoggingSystem:: SetChannelColor( LoggingChannelID_t channelID, Color color )
{ GetChannel( channelID )->m_SpewColor = color; }

LoggingChannelFlags_t CLoggingSystem:: GetChannelFlags( LoggingChannelID_t channelID ) const
{ return GetChannel( channelID )->m_Flags; }

void CLoggingSystem:: SetChannelFlags( LoggingChannelID_t channelID, LoggingChannelFlags_t flags )
{ GetChannel( channelID )->m_Flags = flags; }

bool CLoggingSystem::LoggingChannel_t:: HasTag( const char *pTag ) const
{
	LoggingTag_t *pCurrentTag = m_pFirstTag;
	while( pCurrentTag != NULL )
	{
		if ( stricmp( pCurrentTag->m_pTagName, pTag ) == 0 )
		{
			return true;
		}
		pCurrentTag = pCurrentTag->m_pNextTag;
	}
	return false;
}

bool CLoggingSystem::LoggingChannel_t:: IsEnabled( LoggingVerbosity_t verbosity ) const
{ return verbosity <= m_Verbosity; }

void CLoggingSystem::LoggingChannel_t:: SetVerbosity( LoggingVerbosity_t verbosity )
{ 
	m_Verbosity = verbosity;
	m_nModifiedFields = static_cast<ModificationFlags_t>(m_nModifiedFields | MODIFIED_VERBOSITY);
}
