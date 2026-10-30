#ifndef GAMESESSIONCONFIGURATION_H
#define GAMESESSIONCONFIGURATION_H

#ifdef _WIN32
#pragma once
#endif

#include "networkbasetypes.pb.h"
#include "public/networksystem/netmessages.h"

class GameSessionConfiguration_t : public CBaseCmdKeyValues<CSVCMsg_GameSessionConfiguration>
{
};

#endif