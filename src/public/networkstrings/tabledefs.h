#ifndef NETWORKSTRINGTABLEDEFS_H
#define NETWORKSTRINGTABLEDEFS_H
#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "public/tier0/utldelegate.h"
#include "public/tier0/utlvector.h"
#include "public/tier0/utlstring.h"

typedef int TABLEID;

#define INVALID_STRING_TABLE -1
const unsigned int INVALID_STRING_INDEX = (unsigned int)-1;

// table index is sent in log2(MAX_TABLES) bits
#define MAX_TABLES	32  // Table id is 4 bits

#define INTERFACENAME_NETWORKSTRINGTABLESERVER "Source2EngineToServerStringTable001"
#define INTERFACENAME_NETWORKSTRINGTABLECLIENT "Source2EngineToClientStringTable001"

class INetworkStringTable;

template <typename Signature>
class CUtlDelegate;

class SetStringUserDataRequest_t
{
public:
	void* m_pRawData;
	unsigned int m_cbDataSize;
};

class StringUserData_t
{
public:
	unsigned int m_cbDataSize;
	void* m_pRawData;
};

typedef CUtlDelegate<void ( INetworkStringTable *pTable, int stringNumber, const char *pString, const SetStringUserDataRequest_t *pUserData )> StringChangedDelegate_t;
// Returns false to fall back to a hex dump of the user data
typedef CUtlDelegate<bool ( const char *pString, const SetStringUserDataRequest_t *pUserData, CUtlVector<CUtlString> *pOutLines )> UserDataFormatterDelegate_t;

class INetworkStringTable
{
public:

	virtual					~INetworkStringTable( void );
		
	// Table Info
	virtual const char		*GetTableName( void ) const = 0;
	virtual TABLEID			GetTableId( void ) const = 0;
	virtual int				GetNumStrings( void ) const = 0;

	// Networking
	virtual void			SetStringChangeTick( int tick_count, void *unknown ) = 0;
	virtual int				GetTick( void ) = 0;
	virtual bool			ChangedBetweenTicks(int tickA, int tickB ) const = 0;

	virtual int				AddString( bool bIsServer, const char *value, const SetStringUserDataRequest_t* userdata = 0 ) = 0;

	virtual const char		*GetString( int stringNumber, bool bFailSilent = false ) const = 0;
	virtual bool			SetStringUserData(int stringNumber, const SetStringUserDataRequest_t *userdata, bool bForceOverride) = 0;
	virtual const StringUserData_t* GetStringUserData(int stringNumber) const = 0;
	virtual int				FindStringIndex( char const *string ) = 0; // returns INVALID_STRING_INDEX if not found
	virtual void			SetStringChangedCallback( const StringChangedDelegate_t &callback, bool bCallForExisting ) = 0;
	virtual void			SetAllowClientSideAddString( bool state ) = 0;
	virtual void			SetDebugLogging( bool bDebug ) = 0;
	virtual void			SetDebugLoggingPrefix( const char *pszPrefix ) = 0;
	virtual void			SetUserDataFormatter( const UserDataFormatterDelegate_t &formatter ) = 0;
};

enum ENetworkStringtableFlags
{
	NSF_NONE = 0,
	NSF_DICTIONARY_ENABLED  = (1<<0), // Uses pre-calculated per map dictionaries to reduce bandwidth
};

class StringTableInit_t
{
public:
	const char *m_pTableName;
	// Size in bytes of every string's user data, 0 when it varies; at most 16384 without m_bUsingVarintBitCounts
	int m_nUserDataFixedSize;
	// At most 17 without m_bUsingVarintBitCounts
	int m_nUserDataNetworkBits;
	int m_nFlags; // ENetworkStringtableFlags
	bool m_bUsingVarintBitCounts;
};

class INetworkStringTableContainer: public IAppSystem
{
public:
	
	virtual					~INetworkStringTableContainer( void );
	
	// table creation/destruction
	virtual INetworkStringTable	*CreateStringTable( StringTableInit_t *initClass ) = 0;
	virtual void				RemoveAllTables( void ) = 0;
	
	// table infos
	virtual INetworkStringTable	*FindTable( const char *tableName ) const = 0;
	virtual INetworkStringTable	*GetTable( TABLEID stringTable ) const = 0;
	virtual int					GetNumTables( void ) const = 0;

	virtual const char			*GetName( void ) const = 0;
};

#endif