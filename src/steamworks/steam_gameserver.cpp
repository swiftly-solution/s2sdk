#include "public/steamworks/steam_gameserver.h"

bool  SteamGameServer_Init( uint32 unIP, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString )
{
	return SteamGameServer_InitEx( unIP, usGamePort, usQueryPort, eServerMode, pchVersionString, NULL ) == k_ESteamAPIInitResult_OK;
}

ESteamAPIInitResult  SteamGameServer_InitEx( uint32 unIP, uint16 usGamePort, uint16 usQueryPort, EServerMode eServerMode, const char *pchVersionString, SteamErrMsg *pOutErrMsg )
{
	const char *pszInternalCheckInterfaceVersions = 
		STEAMUTILS_INTERFACE_VERSION "\0"
		STEAMNETWORKINGUTILS_INTERFACE_VERSION "\0"

		STEAMGAMESERVER_INTERFACE_VERSION "\0"
		STEAMGAMESERVERSTATS_INTERFACE_VERSION "\0"
		STEAMHTTP_INTERFACE_VERSION "\0"
		STEAMINVENTORY_INTERFACE_VERSION "\0"
		STEAMNETWORKING_INTERFACE_VERSION "\0"
		STEAMNETWORKINGMESSAGES_INTERFACE_VERSION "\0"
		STEAMNETWORKINGSOCKETS_INTERFACE_VERSION "\0"
		STEAMUGC_INTERFACE_VERSION "\0"
		"\0";
	return SteamInternal_GameServer_Init_V2( unIP, usGamePort, usQueryPort, eServerMode, pchVersionString, pszInternalCheckInterfaceVersions, pOutErrMsg );
}

void  SteamGameServer_ReleaseCurrentThreadMemory()
{
	SteamAPI_ReleaseCurrentThreadMemory();
}
