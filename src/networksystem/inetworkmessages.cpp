#include "public/networksystem/inetworkmessages.h"

CNetMessage::~CNetMessage()
{}

float CNetMessage:: GetMargin() const
{
	return m_flMargin;
}
