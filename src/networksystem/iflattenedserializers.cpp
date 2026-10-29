#include "public/networksystem/iflattenedserializers.h"

SendProxyRecipientsDelta_t::SendProxyRecipientsDelta_t()
: m_nPlayerSlot( -1 ), m_pFrom( nullptr ), m_pTo( nullptr ), m_bHasNewlyReceived( false )
{}
