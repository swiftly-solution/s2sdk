#include "public/tier0/loggingtypes.h"

void ILoggingListener:: OnFlush()
{ }

void ILoggingListener:: OnChannelRegistered( LoggingChannelID_t channelID )
{ }

void ILoggingListener:: OnChannelVerbosityChanged( LoggingChannelID_t channelID )
{ }

void ILoggingListener:: OnChannelFlagsChanged( LoggingChannelID_t channelID )
{ }
