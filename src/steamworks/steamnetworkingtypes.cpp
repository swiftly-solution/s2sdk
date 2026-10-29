#include "public/steamworks/steamnetworkingtypes.h"

bool SteamNetworkingIPAddr:: IsFakeIP() const
{ return GetFakeIPType() > k_ESteamNetworkingFakeIPType_NotFake; }

bool SteamNetworkingIdentity:: IsFakeIP() const
{ return GetFakeIPType() > k_ESteamNetworkingFakeIPType_NotFake; }

SteamNetworkingMessage_t::~SteamNetworkingMessage_t()
{}

void SteamNetworkingConfigValue_t:: SetInt32( ESteamNetworkingConfigValue eVal, int32_t data )
{
	m_eValue = eVal;
	m_eDataType = k_ESteamNetworkingConfig_Int32;
	m_val.m_int32 = data;
}

void SteamNetworkingConfigValue_t:: SetInt64( ESteamNetworkingConfigValue eVal, int64_t data )
{
	m_eValue = eVal;
	m_eDataType = k_ESteamNetworkingConfig_Int64;
	m_val.m_int64 = data;
}

void SteamNetworkingConfigValue_t:: SetFloat( ESteamNetworkingConfigValue eVal, float data )
{
	m_eValue = eVal;
	m_eDataType = k_ESteamNetworkingConfig_Float;
	m_val.m_float = data;
}

void SteamNetworkingConfigValue_t:: SetPtr( ESteamNetworkingConfigValue eVal, void *data )
{
	m_eValue = eVal;
	m_eDataType = k_ESteamNetworkingConfig_Ptr;
	m_val.m_ptr = data;
}

void SteamNetworkingConfigValue_t:: SetString( ESteamNetworkingConfigValue eVal, const char *data )
{
	m_eValue = eVal;
	m_eDataType = k_ESteamNetworkingConfig_Ptr;
	m_val.m_string = data;
}

SteamNetworkingPOPID  CalculateSteamNetworkingPOPIDFromString( const char *pszCode )
{
	// OK we made a bad decision when we decided how to pack 3-character codes into a uint32.  We'd like to support
	// 4-character codes, but we don't want to break compatibility.  The migration path has some subtleties that make
	// this nontrivial, and there are already some IDs stored in SQL.  Ug, so the 4 character code "abcd" will
	// be encoded with the digits like "0xddaabbcc".
	//
	// Also: we don't currently use 1- or 2-character codes, but if ever do in the future, let's make sure don't read
	// past the end of the string and access uninitialized memory.  (And if the string is empty, we always want
	// to return 0 and not read bytes past the '\0'.)
	//
	// There is also extra paranoia to make sure the bytes are not treated as signed.
	SteamNetworkingPOPID result = (uint32)(uint8)pszCode[0] << 16U;
	if ( result && pszCode[1] )
	{
		result |= ( (uint32)(uint8)pszCode[1] << 8U );
		if ( pszCode[2] )
		{
			result |= (uint32)(uint8)pszCode[2] | ( (uint32)(uint8)pszCode[3] << 24U );
		}
	}
	return result;
}
