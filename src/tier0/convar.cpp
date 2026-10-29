#include "public/tier0/convar.h"
#include "public/tier0/icvar.h"
#include "public/tier0/stdlib.h"

int CCommand::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

int CCommand::ArgC() const
{
	return m_Args.Count();
}

const char **CCommand::ArgV() const
{
	return ArgC() ? (const char**)m_Args.Base() : NULL;
}

const char *CCommand::ArgS() const
{
	return m_nArgv0Size ? (m_ArgSBuffer.Base() + m_nArgv0Size) : "";
}

const char *CCommand::GetCommandString() const
{
	return ArgC() ? m_ArgSBuffer.Base() : "";
}

const char *CCommand::Arg( int nIndex ) const
{
    return (nIndex >= 0 && nIndex < ArgC() ? m_Args[nIndex] : "");
}

const char *CCommand::operator[]( int nIndex ) const
{
	return Arg( nIndex );
}

CVarTypeTraits *GetCvarTypeTraits( EConVarType type )
{
	Assert( type >= 0 && type < EConVarType_MAX );

	static CVarTypeTraits s_TypeTraits[EConVarType_MAX] = {
		*CVarTypeTraits().InitAs<bool>( "bool", "false" ),
		*CVarTypeTraits().InitAs<int16_t>( "int16", "0" ),
		*CVarTypeTraits().InitAs<uint16_t>( "uint16", "0" ),
		*CVarTypeTraits().InitAs<int32_t>( "int32", "0" ),
		*CVarTypeTraits().InitAs<uint32_t>( "uint32", "0" ),
		*CVarTypeTraits().InitAs<int64_t>( "int64", "0" ),
		*CVarTypeTraits().InitAs<uint64_t>( "uint64", "0" ),
		*CVarTypeTraits().InitAs<float>( "float32", "0" ),
		*CVarTypeTraits().InitAs<double>( "float64", "0" ),
		*CVarTypeTraits().InitAs<CUtlString>( "string", "" ),
		*CVarTypeTraits().InitAs<Color>( "color", "0 0 0 255" ),
		*CVarTypeTraits().InitAs<Vector2D>( "vector2", "0 0" ),
		*CVarTypeTraits().InitAs<Vector>( "vector3", "0 0 0" ),
		*CVarTypeTraits().InitAs<Vector4D>( "vector4", "0 0 0 0" ),
		*CVarTypeTraits().InitAs<QAngle>( "qangle", "0 0 0" ),
		*CVarTypeTraits().InitAs<VectorWS>( "vectorws", "0 0 0" )
	};

	return &s_TypeTraits[type];
}

ConVarData *GetInvalidConVarData( EConVarType type )
{
	Assert( type >= EConVarType_Invalid && type < EConVarType_MAX );

	static ConVarData s_InvalidConVar[EConVarType_MAX + 1] = {
		ConVarData( TranslateConVarType<bool>() ),
		ConVarData( TranslateConVarType<int16_t>() ),
		ConVarData( TranslateConVarType<uint16_t>() ),
		ConVarData( TranslateConVarType<int32_t>() ),
		ConVarData( TranslateConVarType<uint32_t>() ),
		ConVarData( TranslateConVarType<int64_t>() ),
		ConVarData( TranslateConVarType<uint64_t>() ),
		ConVarData( TranslateConVarType<float>() ),
		ConVarData( TranslateConVarType<double>() ),
		ConVarData( TranslateConVarType<CUtlString>() ),
		ConVarData( TranslateConVarType<Color>() ),
		ConVarData( TranslateConVarType<Vector2D>() ),
		ConVarData( TranslateConVarType<Vector>() ),
		ConVarData( TranslateConVarType<Vector4D>() ),
		ConVarData( TranslateConVarType<QAngle>() ),
		ConVarData( TranslateConVarType<VectorWS>() ),
		ConVarData( TranslateConVarType<void *>() ) // EConVarType_MAX
	};

	if(type == EConVarType_Invalid)
	{
		return &s_InvalidConVar[EConVarType_MAX];
	}
	return &s_InvalidConVar[type];
}

static FnConVarRegisterCallback s_ConVarRegCB = nullptr;
static FnConCommandRegisterCallback s_ConCommandRegCB = nullptr;
static uint64_t s_nCVarFlag = 0;
static bool s_bRegistered = false;
static ICvar* g_pCVar = nullptr;

void SetGlobalCvarObject(ICvar* cvar)
{
    g_pCVar = cvar;
}

bool ConCommandRegList::s_bConCommandsRegistered = false;
ConCommandRegList *ConCommandRegList::s_pRoot = nullptr;

void UnRegisterConCommand( ConCommandRef *cmd )
{
	if(cmd->IsValidRef())
	{
		if(g_pCVar)
			g_pCVar->UnregisterConCommandCallbacks( *cmd );

		cmd->InvalidateRef();
	}
}

void ConCommandRegList::RegisterConCommand( const Entry_t &cmd )
{
	*cmd.m_Command = g_pCVar->RegisterConCommand( cmd.m_Info, s_nCVarFlag );
	if(!cmd.m_Command->IsValidRef())
	{
		Plat_FatalError( "RegisterConCommand: Unknown error registering con command \"%s\"!\n", cmd.m_Info.m_pszName );
		DebuggerBreak();
	}
	else if(s_ConCommandRegCB)
		s_ConCommandRegCB( cmd.m_Command );
}

void ConCommandRegList::RegisterAll()
{
	if(!s_bConCommandsRegistered && g_pCVar)
	{
		s_bConCommandsRegistered = true;

		ConCommandRegList *prev = nullptr;
		for(auto list = s_pRoot; list; list = prev)
		{
			for(size_t i = 0; i < list->m_nSize; i++)
			{
				RegisterConCommand( list->m_Entries[i] );
			}

			prev = list->m_pPrev;
		};
	}
}

void ConCommandRegList::UnregisterAll()
{
	if(s_bConCommandsRegistered && g_pCVar)
	{
		s_bConCommandsRegistered = false;

		ConCommandRegList *prev = nullptr;
		for(auto list = s_pRoot; list; list = prev)
		{
			for(size_t i = 0; i < list->m_nSize; i++)
			{
				UnRegisterConCommand( list->m_Entries[i].m_Command );
			}

			prev = list->m_pPrev;
			delete list;
		};

		s_pRoot = nullptr;
	}
}

void ConCommandRegList::AddToList( const Entry_t &cmd )
{
	if(s_bConCommandsRegistered)
	{
		RegisterConCommand( cmd );
		return;
	}

	auto list = s_pRoot;

	if(!list || list->m_nSize >= (sizeof( m_Entries ) / sizeof( m_Entries[0] )))
	{
		list = new ConCommandRegList;
		list->m_nSize = 0;
		list->m_pPrev = s_pRoot;

		s_pRoot = list;
	}

	list->m_Entries[list->m_nSize++] = cmd;
}

void SetupConCommand( ConCommand *cmd, const ConCommandCreation_t& info )
{
	ConCommandRegList::Entry_t entry;
	entry.m_Info = info;
	entry.m_Command = cmd;

	ConCommandRegList::AddToList( entry );
}

bool ConVarRegList::s_bConVarsRegistered = false;
ConVarRegList *ConVarRegList::s_pRoot = nullptr;

void UnRegisterConVar( ConVarRef *cvar )
{
	if(cvar->IsValidRef())
	{
		if(g_pCVar)
			g_pCVar->UnregisterConVarCallbacks( *cvar );

		cvar->InvalidateRef();
	}
}

void ConVarRegList::RegisterConVar( const Entry_t &cvar )
{
	g_pCVar->RegisterConVar( cvar.m_Info, s_nCVarFlag, cvar.m_pConVar, cvar.m_pConVarData );
	if(!cvar.m_pConVar->IsValidRef())
	{
		Plat_FatalError( "RegisterConVar: Unknown error registering convar \"%s\"!\n", cvar.m_Info.m_pszName );
		DebuggerBreak();
	}
	// Don't let references pass as a newly registered cvar
	else if(s_ConVarRegCB && (cvar.m_Info.m_nFlags & CVarFlags::REFERENCE) == 0)
		s_ConVarRegCB( cvar.m_pConVar );
}

void ConVarRegList::RegisterAll()
{
	if(!s_bConVarsRegistered && g_pCVar)
	{
		s_bConVarsRegistered = true;

		ConVarRegList *prev = nullptr;
		for(auto list = s_pRoot; list; list = prev)
		{
			for(size_t i = 0; i < list->m_nSize; i++)
			{
				RegisterConVar( list->m_Entries[i] );
			}

			prev = list->m_pPrev;
		};
	}
}

void ConVarRegList::UnregisterAll()
{
	if(s_bConVarsRegistered && g_pCVar)
	{
		s_bConVarsRegistered = false;

		ConVarRegList *prev = nullptr;
		for(auto list = s_pRoot; list; list = prev)
		{
			for(size_t i = 0; i < list->m_nSize; i++)
			{
				UnRegisterConVar( list->m_Entries[i].m_pConVar );
			}

			prev = list->m_pPrev;
			delete list;
		};

		s_pRoot = nullptr;
	}
}

void ConVarRegList::AddToList( const Entry_t &cvar )
{
	if(s_bConVarsRegistered)
	{
		RegisterConVar( cvar );
		return;
	}

	auto list = s_pRoot;

	if(!list || list->m_nSize >= (sizeof( m_Entries ) / sizeof( m_Entries[0] )))
	{
		list = new ConVarRegList;
		list->m_nSize = 0;
		list->m_pPrev = s_pRoot;

		s_pRoot = list;
	}

	list->m_Entries[list->m_nSize++] = cvar;
}

void SetupConVar( ConVarRefAbstract *cvar, ConVarData **cvar_data, ConVarCreation_t &info )
{
	ConVarRegList::Entry_t entry;
	entry.m_Info = info;
	entry.m_pConVar = cvar;
	entry.m_pConVarData = cvar_data;

	ConVarRegList::AddToList( entry );
}

uint64_t SanitiseConVarFlags( uint64_t flags )
{
    return flags;
}

void ConVar_Register( uint64 nCVarFlag, FnConVarRegisterCallback cvar_reg_cb, FnConCommandRegisterCallback cmd_reg_cb )
{
	if ( !g_pCVar || s_bRegistered )
	{
		return;
	}

	s_bRegistered = true;
	s_nCVarFlag = nCVarFlag;
	s_ConVarRegCB = cvar_reg_cb;
	s_ConCommandRegCB = cmd_reg_cb;

	ConCommandRegList::RegisterAll();
	ConVarRegList::RegisterAll();
}

void ConVar_Unregister( )
{
	if ( !g_pCVar || !s_bRegistered )
		return;

	s_bRegistered = false;
	s_nCVarFlag = CVarFlags::NONE;
	s_ConVarRegCB = nullptr;
	s_ConCommandRegCB = nullptr;

	ConCommandRegList::UnregisterAll();
	ConVarRegList::UnregisterAll();
}

CCommand::CCommand()
{
	m_ArgSBuffer.SetCount( COMMAND_MAX_LENGTH );
	m_ArgvBuffer.SetCount( COMMAND_MAX_LENGTH );
	Reset();
}

CCommand::CCommand( int nArgC, const char **ppArgV ) : CCommand()
{
	Assert( nArgC > 0 );

	char *pBuf = m_ArgvBuffer.Base();
	char *pSBuf = m_ArgSBuffer.Base();
	const char *pBufEnd = pBuf + m_ArgvBuffer.Count();
	const char *pSBufEnd = pSBuf + m_ArgSBuffer.Count();
	for ( int i = 0; i < nArgC; ++i )
	{
		int nLen = V_strlen( ppArgV[i] );
		bool bContainsSpace = strchr( ppArgV[i], ' ' ) != NULL;

		// Each argument is followed by a separator or the terminator in the ArgS buffer
		if ( nLen + 1 > pBufEnd - pBuf || nLen + ( bContainsSpace ? 2 : 0 ) + 1 > pSBufEnd - pSBuf )
		{
			Warning( "CCommand::CCommand: Encountered command which overflows the tokenizer buffer.. Skipping!\n" );
			Reset();
			return;
		}

		m_Args.AddToTail( pBuf );
		memcpy( pBuf, ppArgV[i], nLen+1 );
		pBuf += nLen+1;

		if ( bContainsSpace )
		{
			*pSBuf++ = '\"';
		}
		memcpy( pSBuf, ppArgV[i], nLen );
		pSBuf += nLen;
		if ( bContainsSpace )
		{
			*pSBuf++ = '\"';
		}

		if ( i != nArgC - 1 )
		{
			*pSBuf++ = ' ';
		}

		if ( i == 0 && nArgC > 1 )
		{
			m_nArgv0Size = pSBuf - m_ArgSBuffer.Base();
		}
	}

	*pSBuf = '\0';
}

CCommand &CCommand::operator=( const CCommand &other )
{
	if ( this == &other )
		return *this;

	m_ArgSBuffer.SetCount( other.m_ArgvBuffer.Count() );
	m_ArgvBuffer.SetCount( other.m_ArgvBuffer.Count() );
	Tokenize( other.GetCommandString() );
	m_unk101 = other.m_unk101;
	m_flInputTime = other.m_flInputTime;
	m_unk201 = other.m_unk201;
	return *this;
}

void CCommand::EnsureBuffers()
{
	m_ArgSBuffer.SetSize( MaxCommandLength() );
	m_ArgvBuffer.SetSize( MaxCommandLength() );
}

void CCommand::Reset()
{
	m_nArgv0Size = 0;
	m_ArgSBuffer.Base()[0] = '\0';
	m_Args.RemoveAll();
	m_unk101 = false;
	m_flInputTime = std::numeric_limits<double>::quiet_NaN();
	m_unk201 = 0;
}

characterset_t* CCommand::DefaultBreakSet()
{
	return g_pCVar->GetCharacterSet();
}

bool CCommand::Tokenize( CUtlString pCommand, characterset_t *pBreakSet )
{
	if(m_ArgSBuffer.Count() == 0)
		EnsureBuffers();

	Reset();

	if ( pCommand.IsEmpty() )
		return false;

	// Use default break set
	if ( !pBreakSet )
	{
		pBreakSet = DefaultBreakSet();
	}

	// Copy the current command into a temp buffer
	// NOTE: This is here to avoid the pointers returned by DequeueNextCommand
	// to become invalid by calling AddText. Is there a way we can avoid the memcpy?
	int nLen = pCommand.Length();
	if ( nLen >= m_ArgSBuffer.Count() - 1 )
	{
		Warning( "CCommand::Tokenize: Encountered command which overflows the tokenizer buffer.. Skipping!\n" );
		return false;
	}

	memmove( m_ArgSBuffer.Base(), pCommand, nLen + 1 );

	// Parse the current command into the current command buffer
	CUtlBuffer bufParse( m_ArgSBuffer.Base(), nLen, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
	int nArgvBufferSize = 0;
	while ( bufParse.IsValid() )
	{
		char *pArgvBuf = &m_ArgvBuffer[nArgvBufferSize];
		int nMaxLen = m_ArgvBuffer.Count() - nArgvBufferSize;
		int nStartGet = bufParse.TellGet();
		int	nSize = bufParse.ParseToken( pBreakSet, pArgvBuf, nMaxLen );
		if ( nSize < 0 )
			break;

		// Check for overflow condition
		if ( nMaxLen == nSize )
		{
			Reset();
			return false;
		}

		if ( m_Args.Count() == 1 )
		{
			// Deal with the case where the arguments were quoted
			m_nArgv0Size = bufParse.TellGet();
			bool bFoundEndQuote = m_ArgSBuffer[m_nArgv0Size-1] == '\"';
			if ( bFoundEndQuote )
			{
				--m_nArgv0Size;
			}
			m_nArgv0Size -= nSize;
			Assert( m_nArgv0Size != 0 );

			// The StartGet check is to handle this case: "foo"bar
			// which will parse into 2 different args. ArgS should point to bar.
			bool bFoundStartQuote = ( m_nArgv0Size > nStartGet ) && ( m_ArgSBuffer[m_nArgv0Size-1] == '\"' );
			Assert( bFoundEndQuote == bFoundStartQuote );
			if ( bFoundStartQuote )
			{
				--m_nArgv0Size;
			}
		}

		m_Args.AddToTail( pArgvBuf );
		nArgvBufferSize += nSize + 1;
		
		if(nArgvBufferSize >= m_ArgvBuffer.Count())
			break;
	}

	return true;
}

int CCommand::FindArg( const char *pName ) const
{
	int nArgC = ArgC();
	for ( int i = 1; i < nArgC; i++ )
	{
		if ( !V_stricmp_fast( Arg(i), pName ) )
			return (i+1) < nArgC ? i+1 : -1;
	}
	return -1;
}

int CCommand::FindArgInt( const char *pName, int nDefaultVal ) const
{
	int idx = FindArg( pName );
	if ( idx != -1 )
		return V_atoi( m_Args[idx] );
	else
		return nDefaultVal;
}

ConCommandRef::ConCommandRef( const char *name, bool allow_developer )
{
	*this = g_pCVar->FindConCommand( name, allow_developer );
}

ConCommandData *ConCommandRef::GetRawData()
{
	return g_pCVar->GetConCommandData( *this );
}

void ConCommandRef::Dispatch( const CCommandContext &context, const CCommand &command )
{
	g_pCVar->DispatchConCommand( *this, context, command );
}

void ConCommand::Create( const char* pName, const ConCommandCallbackInfo_t &cb, const char* pHelpString, uint64 flags, const CompletionCallbackInfo_t &completion_cb )
{
	// Name should be static data
	Assert(pName);

	ConCommandCreation_t info;
	info.m_pszName = pName;
	info.m_pszHelpString = pHelpString;
	info.m_nFlags = SanitiseConVarFlags( flags );
	info.m_CBInfo = cb;
	info.m_CompletionCBInfo = completion_cb;

	SetupConCommand( this, info );
}

void ConCommand::Destroy()
{
	UnRegisterConCommand( this );
}

int ConVarData::GetMaxSplitScreenSlots() const
{
	if((m_nFlags & CVarFlags::PER_USER) != 0)
		return g_pCVar->GetMaxSplitScreenSlots();

	return 1;
}

CVValue_t *ConVarData::Value( CSplitScreenSlot slot ) const
{
	if(slot.Get() == -1)
		slot = CSplitScreenSlot( 0 );

	if(!IsSlotInRange( slot ))
		return nullptr;

	return (CVValue_t*)&m_Value;
}

CVValue_t *ConVarData::ValueOrDefault( CSplitScreenSlot slot ) const
{
	CVValue_t *value = Value( slot );
	return value ? value : m_defaultValue;
}

bool ConVarData::IsAllSetToDefault() const
{
	for(int i = 0; i < GetMaxSplitScreenSlots(); i++)
	{
		if(!IsSetToDefault( i ))
			return false;
	}

	return true;
}

void ConVarData::MinValueToString( CBufferString &buf ) const
{
	if(HasMinValue())
		TypeTraits()->ValueToString( m_minValue, buf );
	else
		buf.Insert( 0, "" );
}

void ConVarData::MaxValueToString( CBufferString &buf ) const
{
	if(HasMaxValue())
		TypeTraits()->ValueToString( m_maxValue, buf );
	else
		buf.Insert( 0, "" );
}

ConVarRef::ConVarRef( const char *name, bool allow_developer )
{
	*this = g_pCVar->FindConVar( name, allow_developer );
}

void ConVarRefAbstract::Init( ConVarRef ref, EConVarType type )
{
	m_ConVarData = nullptr;

	if(g_pCVar)
		m_ConVarData = g_pCVar->GetConVarData( ref );
	
	if(!m_ConVarData)
		InvalidateConVarData( type );
}

void ConVarRefAbstract::InvalidateConVarData( EConVarType type )
{
	InvalidateRef();

	if(type == EConVarType_Invalid)
		m_ConVarData = GetInvalidConVarData( EConVarType_Invalid );
	else
		m_ConVarData = GetCvarTypeTraits( type )->m_InvalidCvarData;
}

void ConVarRefAbstract::CallChangeCallbacks( CSplitScreenSlot slot, CVValue_t *new_value, CVValue_t *prev_value, const char *new_str, const char *prev_str )
{
	if(slot.Get() == -1)
		slot = CSplitScreenSlot( 0 );

	g_pCVar->CallChangeCallback( *this, slot, new_value, prev_value );
	g_pCVar->CallGlobalChangeCallbacks( this, slot, new_str, prev_str );
}

void ConVarRefAbstract::SetOrQueueValueInternal( CSplitScreenSlot slot, CVValue_t *value )
{
	if(m_ConVarData->IsFlagSet( CVarFlags::PERFORMING_CALLBACKS ))
		QueueSetValueInternal( slot, value );
	else
		SetValueInternal( slot, value );
}

void ConVarRefAbstract::QueueSetValueInternal( CSplitScreenSlot slot, CVValue_t *value )
{
	if(slot.Get() == -1)
		slot = CSplitScreenSlot( 0 );

	TypeTraits()->Clamp( value, m_ConVarData->MinValue(), m_ConVarData->MaxValue() );

	if(!m_ConVarData->IsEqual( slot, value ))
		g_pCVar->QueueThreadSetValue( this, slot, nullptr, value );
}

void ConVarRefAbstract::SetValueInternal( CSplitScreenSlot slot, CVValue_t *value )
{
	CVValue_t *curr_value = m_ConVarData->ValueOrDefault( slot );

	CVValue_t prev;
	TypeTraits()->Construct( &prev );
	TypeTraits()->Copy( &prev, *curr_value );

	if(g_pCVar->CallFilterCallback( *this, slot, value, &prev ))
	{
		TypeTraits()->Destruct( curr_value );

		TypeTraits()->Construct( curr_value );
		TypeTraits()->Copy( curr_value, *value );
		m_ConVarData->Clamp( slot );

		if(!m_ConVarData->IsEqual( slot, &prev ))
		{
			CBufferString prev_str, new_str;

			TypeTraits()->ValueToString( &prev, prev_str );
			TypeTraits()->ValueToString( curr_value, new_str );

			m_ConVarData->IncrementTimesChanged();

			CallChangeCallbacks( slot, curr_value, &prev, new_str.Get(), prev_str.Get() );
		}
	}

	TypeTraits()->Destruct( &prev );
}

bool ConVarRefAbstract::SetString( CUtlString string, CSplitScreenSlot slot )
{
	CVValue_t *value = m_ConVarData->Value( slot );

	if(!value)
		return true;
	
	if(GetType() != EConVarType_String)
		string.Trim( "\t\n\v\f\r " );

	CVValue_t new_value;
	TypeTraits()->Construct( &new_value );

	bool success = false;
	if(TypeTraits()->StringToValue( string.Get(), &new_value ))
	{
		SetOrQueueValueInternal( slot, &new_value );
		success = true;
	}

	TypeTraits()->Destruct( &new_value );
	return success;
}

void ConVarRefAbstract::Revert( CSplitScreenSlot slot )
{
	CBufferString buf;
	GetDefaultAsString( buf );
	SetString( buf.Get(), slot );
}

void ConVar_PrintDescription( const ConVarRefAbstract *ref )
{
	Assert( ref );

	static struct
	{
		uint64 m_Flag;
		const char *m_Name;
	} s_FlagsMap[] = {
		{ CVarFlags::GAMEDLL, "game" },
		{ CVarFlags::CLIENTDLL, "client" },
		{ CVarFlags::ARCHIVE, "archive" },
		{ CVarFlags::NOTIFY, "notify" },
		{ CVarFlags::SPONLY, "singleplayer" },
		{ CVarFlags::NOT_CONNECTED, "notconnected" },
		{ CVarFlags::CHEAT, "cheat" },
		{ CVarFlags::REPLICATED, "replicated" },
		{ CVarFlags::SERVER_CAN_EXECUTE, "server_can_execute" },
		{ CVarFlags::CLIENTCMD_CAN_EXECUTE, "clientcmd_can_execute" },
		{ CVarFlags::USERINFO, "userinfo" },
		{ CVarFlags::PER_USER, "per_user" }
	};

	CBufferStringN<4096> desc;
	CBufferString buf;

	ref->GetValueAsString( buf );
	desc.AppendFormat( "\"%s\" = \"%s\"", ref->GetName(), buf.Get() );

	if(!ref->IsSetToDefault())
	{
		ref->GetDefaultAsString( buf );
		desc.AppendFormat( " ( def. \"%s\" )", buf.Get() );
	}

	if(ref->HasMin())
	{
		ref->GetMinAsString( buf );
		desc.AppendFormat( " min. %s", buf.Get() );
	}

	if(ref->HasMax())
	{
		ref->GetMaxAsString( buf );
		desc.AppendFormat( " max. %s", buf.Get() );
	}

	for(size_t i = 0; i < (sizeof( s_FlagsMap ) / sizeof( s_FlagsMap[0] )); i++)
	{
		if(ref->IsFlagSet( s_FlagsMap[i].m_Flag ))
			desc.AppendFormat( " %s", s_FlagsMap[i].m_Name );
	}

	if(ref->HasHelpText())
		ConMsg( "%-120s - %s\n", desc.Get(), ref->GetHelpText() );
	else
		ConMsg( "%-120s\n", desc.Get() );
}

CVarCreationBase_t::CVarCreationBase_t()
:
	m_pszName( nullptr ),
	m_pszHelpString( nullptr ),
	m_nFlags( 0 )
{}

CCommandContext::CCommandContext( CommandTarget_t nTarget, CPlayerSlot nSlot )
:
	m_nTarget( nTarget ), m_nPlayerSlot( nSlot )
{}

CommandTarget_t CCommandContext::GetTarget() const
{
	return m_nTarget;
}

CPlayerSlot CCommandContext::GetPlayerSlot() const
{
	return m_nPlayerSlot;
}

ConCommandCallbackInfo_t::ConCommandCallbackInfo_t()
:
	m_fnCommandCallback( nullptr ),
	m_bIsInterface( false ),
	m_bIsVoidCallback( false ),
	m_bIsContextLess( false )
{}

ConCommandCallbackInfo_t::ConCommandCallbackInfo_t( FnCommandCallback_t cb )
:
	m_fnCommandCallback( cb ),
	m_bIsInterface( false ),
	m_bIsVoidCallback( false ),
	m_bIsContextLess( false )
{}

ConCommandCallbackInfo_t::ConCommandCallbackInfo_t( FnCommandCallbackVoid_t cb )
:
	m_fnVoidCommandCallback( cb ),
	m_bIsInterface( false ),
	m_bIsVoidCallback( cb ? true : false ),
	m_bIsContextLess( false )
{}

ConCommandCallbackInfo_t::ConCommandCallbackInfo_t( FnCommandCallbackNoContext_t cb )
:
	m_fnContextlessCommandCallback( cb ),
	m_bIsInterface( false ),
	m_bIsVoidCallback( false ),
	m_bIsContextLess( cb ? true : false )
{}

ConCommandCallbackInfo_t::ConCommandCallbackInfo_t( ICommandCallback *cb )
:
	m_pCommandCallback( cb ),
	m_bIsInterface( cb ? true : false ),
	m_bIsVoidCallback( false ),
	m_bIsContextLess( false )
{}

bool ConCommandCallbackInfo_t::IsValid() const
{ return m_fnCommandCallback != nullptr; }

void ConCommandCallbackInfo_t::Dispatch( const CCommandContext &context, const CCommand &command ) const
{
	if(!IsValid())
		return;

	if(m_bIsInterface)
		m_pCommandCallback->CommandCallback( context, command );
	else if(m_bIsVoidCallback)
		m_fnVoidCommandCallback();
	else if(m_bIsContextLess)
		m_fnContextlessCommandCallback( command );
	else
		m_fnCommandCallback( context, command );
}

CompletionCallbackInfo_t::CompletionCallbackInfo_t()
:
	m_fnCompletionCallback( nullptr ),
	m_bIsInterface( false )
{}

CompletionCallbackInfo_t::CompletionCallbackInfo_t( FnCommandCompletionCallback cb )
:
	m_fnCompletionCallback( cb ),
	m_bIsInterface( false )
{}

CompletionCallbackInfo_t::CompletionCallbackInfo_t( ICommandCompletionCallback *cb )
:
	m_pCommandCompletionCallback( cb ),
	m_bIsInterface( cb ? true : false )
{}

bool CompletionCallbackInfo_t::IsValid() const
{ return m_fnCompletionCallback != nullptr; }

int CompletionCallbackInfo_t::Dispatch( const CCommand &command, CUtlVector< CUtlString > &completions ) const
{
	if(!IsValid())
		return 0;

	if(m_bIsInterface)
		return m_pCommandCompletionCallback->CommandCompletionCallback( command, completions );
	
	m_fnCompletionCallback( command, completions );
	return completions.Count();
}

const char * ConCommandData::GetName() const
{ return m_pszName; }

const char * ConCommandData::GetHelpText() const
{ return m_pszHelpString; }

bool ConCommandData::HasHelpText() const
{ return m_pszHelpString && m_pszHelpString[0]; }

bool ConCommandData::IsFlagSet( uint64_t flag ) const
{ return (m_nFlags & flag) != 0; }

void ConCommandData::AddFlags( uint64_t flags )
{ m_nFlags |= flags; }

void ConCommandData::RemoveFlags( uint64_t flags )
{ m_nFlags &= ~flags; }

uint64_t ConCommandData::GetFlags( void ) const
{ return m_nFlags; }

int ConCommandData::GetAutoCompleteSuggestions( const CCommand &command, CUtlVector< CUtlString > &completions ) const
{
	return m_CompletionCB.Dispatch( command, completions );
}

bool ConCommandData::HasCompletionCallback() const
{ return m_CompletionCB.IsValid(); }

bool ConCommandData::HasCallback() const
{ return m_CallbackInfoIndex > 0; }

const char * ConCommandRef::GetName() const
{ return GetRawData()->GetName(); }

const char * ConCommandRef::GetHelpText() const
{ return GetRawData()->GetHelpText(); }

bool ConCommandRef::HasHelpText() const
{ return GetRawData()->HasHelpText(); }

bool ConCommandRef::IsFlagSet( uint64_t flag ) const
{ return GetRawData()->IsFlagSet( flag ); }

void ConCommandRef::AddFlags( uint64_t flags )
{ GetRawData()->AddFlags( flags ); }

void ConCommandRef::RemoveFlags( uint64_t flags )
{ GetRawData()->RemoveFlags( flags ); }

uint64_t ConCommandRef::GetFlags() const
{ return GetRawData()->GetFlags(); }

bool ConCommandRef::HasCompletionCallback() const
{ return GetRawData()->HasCompletionCallback(); }

bool ConCommandRef::HasCallback() const
{ return GetRawData()->HasCallback(); }

int ConCommandRef::GetCompletionCommandIndex() const
{ return GetRawData()->m_CompletionCBCmdIndex; }

uint16_t ConCommandRef::GetCallbackIndex() const
{ return GetRawData()->m_CallbackInfoIndex; }

int ConCommandRef::GetAutoCompleteSuggestions( const CCommand &command, CUtlVector< CUtlString > &completions ) const
{
	return GetRawData()->GetAutoCompleteSuggestions( command, completions );
}

void ConCommandRef::InvalidateRef()
{ m_CommandAccessIndex = kInvalidAccessIndex; m_CommandRegisteredIndex = 0; }

bool ConCommandRef::IsValidRef() const
{ return m_CommandAccessIndex != kInvalidAccessIndex; }

uint16_t ConCommandRef::GetAccessIndex() const
{ return m_CommandAccessIndex; }

int ConCommandRef::GetRegisteredIndex() const
{ return m_CommandRegisteredIndex; }

ConCommand::ConCommand( const char *pName, ConCommandCallbackInfo_t callback,
	const char *pHelpString, uint64_t flags,
	CompletionCallbackInfo_t completionFunc )
: BaseClass()
{
	Create( pName, callback, pHelpString, flags, completionFunc );
}

ConCommand::~ConCommand()
{
	Destroy();
}

ConVarValueInfo_t::ConVarValueInfo_t( EConVarType type )
:
	m_Version( 0 ),
	m_bHasDefault( false ),
	m_bHasMin( false ),
	m_bHasMax( false ),
	m_defaultValue {},
	m_minValue {},
	m_maxValue {},
	m_fnProviderCallBack( nullptr ),
	m_fnCallBack( nullptr ),
	m_fnProviderFilterCallBack( nullptr ),
	m_fnFilterCallBack( nullptr ),
	m_fnGetEnumInfoHandle( nullptr ),
	m_eVarType( type ),
	m_CompletionCallBack()
{}

void ConVarValueInfo_t::SetCompletionCallback( const CompletionCallbackInfo_t &cb )
{
	m_CompletionCallBack = cb;
}

ConVarData::ConVarData( EConVarType type )
: m_Value {}
{
	Invalidate( type, true );
}

void ConVarData::Invalidate( EConVarType type, bool as_undefined )
{
	if(as_undefined)
		m_pszName = "<undefined>";
	m_defaultValue = CVValue_t::InvalidValue();
	m_minValue = nullptr;
	m_maxValue = nullptr;
	m_pszHelpString = as_undefined ? "This convar is being accessed prior to ConVar_Register being called" : nullptr;
	m_eVarType = type;
	m_Version = 0;
	m_iTimesChanged = 0;
	m_nFlags = CVarFlags::REFERENCE;
	m_iCallbackIndex = 0;
	m_iFilterCBIndex = 0;
	m_iCompletionCBIndex = 0;
	m_GameInfoFlags = 0;
	m_UserInfoByteIndex = 0;
	m_fnGetEnumInfoHandle = nullptr;
}

const char * ConVarData::GetName( void ) const
{ return m_pszName; }

const char * ConVarData::GetHelpText( void ) const
{ return m_pszHelpString; }

bool ConVarData::HasHelpText() const
{ return m_pszHelpString && m_pszHelpString[0]; }

EConVarType ConVarData::GetType() const
{ return m_eVarType; }

short ConVarData::GetVersion() const
{ return m_Version; }

int ConVarData::GetTimesChanged() const
{ return m_iTimesChanged; }

void ConVarData::SetTimesChanged( int val )
{ m_iTimesChanged = val; }

void ConVarData::IncrementTimesChanged()
{ m_iTimesChanged++; }

bool ConVarData::IsFlagSet( uint64_t flag ) const
{ return (m_nFlags & flag) != 0; }

void ConVarData::AddFlags( uint64_t flags )
{ m_nFlags |= flags; }

void ConVarData::RemoveFlags( uint64_t flags )
{ m_nFlags &= ~flags; }

uint64_t ConVarData::GetFlags() const
{ return m_nFlags; }

int ConVarData::GetGameInfoFlags() const
{ return m_GameInfoFlags; }

int ConVarData::GetCallbackIndex() const
{ return m_iCallbackIndex; }

int ConVarData::GetUserInfoByteIndex() const
{ return m_UserInfoByteIndex; }

CVarTypeTraits * ConVarData::TypeTraits() const
{
	Assert( m_eVarType != EConVarType_Invalid );
	return GetCvarTypeTraits( m_eVarType );
}

bool ConVarData::HasEnumInfo() const
{ return m_fnGetEnumInfoHandle != nullptr; }

SchemaMetaInfoHandle_t<CSchemaEnumInfo> ConVarData::GetEnumInfo() const
{ return HasEnumInfo() ? m_fnGetEnumInfoHandle() : SchemaMetaInfoHandle_t<CSchemaEnumInfo> {}; }

int ConVarData::GetDataByteSize() const
{ return TypeTraits()->m_ByteSize; }

bool ConVarData::IsPrimitiveType() const
{ return TypeTraits()->m_IsPrimitive; }

const char * ConVarData::GetDataTypeName() const
{ return TypeTraits()->m_TypeName; }

bool ConVarData::HasDefaultValue() const
{ return m_defaultValue && m_defaultValue != CVValue_t::InvalidValue(); }

bool ConVarData::HasMinValue() const
{ return m_minValue != nullptr; }

bool ConVarData::HasMaxValue() const
{ return m_maxValue != nullptr; }

CVValue_t * ConVarData::DefaultValue() const
{ return m_defaultValue; }

CVValue_t * ConVarData::MinValue() const
{ return m_minValue; }

CVValue_t * ConVarData::MaxValue() const
{ return m_maxValue; }

void ConVarData::RemoveDefaultValue()
{ if(HasDefaultValue()) TypeTraits()->StringToValue( TypeTraits()->m_DefaultValue, m_defaultValue ); }

void ConVarData::RemoveMinValue()
{ m_minValue = nullptr; }

void ConVarData::RemoveMaxValue()
{ m_maxValue = nullptr; }

bool ConVarData::UpdateDefaultValueString( const char *value )
{ return HasDefaultValue() && TypeTraits()->StringToValue( value, m_defaultValue ); }

bool ConVarData::UpdateMinValueString( const char *value )
{ return HasMinValue() && TypeTraits()->StringToValue( value, m_minValue ); }

bool ConVarData::UpdateMaxValueString( const char *value )
{ return HasMaxValue() && TypeTraits()->StringToValue( value, m_maxValue ); }

void ConVarData::SetDefaultValue( CVValue_t *value )
{ TypeTraits()->Copy( m_defaultValue, *value ); }

void ConVarData::SetMinValue( CVValue_t *value )
{ m_minValue = value; }

void ConVarData::SetMaxValue( CVValue_t *value )
{ m_maxValue = value; }

bool ConVarData::IsSetToDefault( CSplitScreenSlot slot ) const
{ return TypeTraits()->Equal( ValueOrDefault( slot ), m_defaultValue ); }

void ConVarData::ValueToString( CSplitScreenSlot slot, CBufferString &buf ) const
{ TypeTraits()->ValueToString( ValueOrDefault( slot ), buf ); }

void ConVarData::DefaultValueToString( CBufferString &buf ) const
{ TypeTraits()->ValueToString( m_defaultValue, buf ); }

void ConVarData::Construct( CSplitScreenSlot slot ) const
{ if(IsSlotInRange( slot )) TypeTraits()->Construct( Value( slot ) ); }

void ConVarData::Destruct( CSplitScreenSlot slot ) const
{ if(IsSlotInRange( slot )) TypeTraits()->Destruct( Value( slot ) ); }

bool ConVarData::IsEqual( CSplitScreenSlot slot, CVValue_t *other ) const
{ return TypeTraits()->Equal( ValueOrDefault( slot ), other ); }

void ConVarData::Clamp( CSplitScreenSlot slot ) const
{ if(IsSlotInRange( slot )) TypeTraits()->Clamp( Value( slot ), m_minValue, m_maxValue ); }

bool ConVarData::IsSlotInRange( CSplitScreenSlot slot ) const
{ return slot.Get() == -1 || (slot.Get() >= 0 && slot.Get() < GetMaxSplitScreenSlots()); }

void ConVarRef::InvalidateRef()
{ m_ConVarAccessIndex = kInvalidAccessIndex; m_ConVarRegisteredIndex = 0; }

bool ConVarRef::IsValidRef() const
{ return m_ConVarAccessIndex != kInvalidAccessIndex; }

uint16_t ConVarRef::GetAccessIndex() const
{ return m_ConVarAccessIndex; }

int ConVarRef::GetRegisteredIndex() const
{ return m_ConVarRegisteredIndex; }

ConVarRefAbstract::ConVarRefAbstract( const char *name, bool allow_defensive )
: BaseClass( name, allow_defensive ), m_ConVarData( nullptr )
{
	Init( *this );
}

ConVarRefAbstract::ConVarRefAbstract( ConVarRef ref, EConVarType type )
: BaseClass( ref ), m_ConVarData( nullptr )
{
	Init( *this, type );
}

ConVarRefAbstract::ConVarRefAbstract( const ConVarRefAbstract &ref )
: BaseClass(), m_ConVarData( nullptr )
{
	CopyRef( ref );
}

const char * ConVarRefAbstract::GetName() const
{ return m_ConVarData->GetName(); }

const char * ConVarRefAbstract::GetHelpText() const
{ return m_ConVarData->GetHelpText(); }

bool ConVarRefAbstract::HasHelpText() const
{ return m_ConVarData->HasHelpText(); }

EConVarType ConVarRefAbstract::GetType() const
{ return m_ConVarData->GetType(); }

bool ConVarRefAbstract::HasDefault() const
{ return m_ConVarData->HasDefaultValue(); }

bool ConVarRefAbstract::HasMin() const
{ return m_ConVarData->HasMinValue(); }

bool ConVarRefAbstract::HasMax() const
{ return m_ConVarData->HasMaxValue(); }

bool ConVarRefAbstract::IsFlagSet( uint64_t flag ) const
{ return m_ConVarData->IsFlagSet( flag ); }

void ConVarRefAbstract::AddFlags( uint64_t flags )
{ m_ConVarData->AddFlags( flags ); }

void ConVarRefAbstract::RemoveFlags( uint64_t flags )
{ return m_ConVarData->RemoveFlags( flags ); }

uint64_t ConVarRefAbstract::GetFlags( void ) const
{ return m_ConVarData->GetFlags(); }

bool ConVarRefAbstract::ShouldBeHidden() const
{ return !m_ConVarData || IsFlagSet( CVarFlags::REFERENCE | CVarFlags::HIDDEN | CVarFlags::DEVELOPMENTONLY ); }

bool ConVarRefAbstract::IsSetToDefault( CSplitScreenSlot slot ) const
{ return m_ConVarData->IsSetToDefault( slot ); }

bool ConVarRefAbstract::IsAllSetToDefault() const
{ return m_ConVarData->IsAllSetToDefault(); }

void ConVarRefAbstract::GetValueAsString( CBufferString &buf, CSplitScreenSlot slot ) const
{ m_ConVarData->ValueToString( slot, buf ); }

void ConVarRefAbstract::GetDefaultAsString( CBufferString &buf ) const
{ m_ConVarData->DefaultValueToString( buf ); }

void ConVarRefAbstract::GetMinAsString( CBufferString &buf ) const
{ m_ConVarData->MinValueToString( buf ); }

void ConVarRefAbstract::GetMaxAsString( CBufferString &buf ) const
{ m_ConVarData->MaxValueToString( buf ); }

bool ConVarRefAbstract::IsValuePrimitive() const
{ return m_ConVarData->GetType() != EConVarType_Invalid && m_ConVarData->IsPrimitiveType(); }

bool ConVarRefAbstract::GetBool( CSplitScreenSlot slot ) const
{ return GetAs<bool>( slot ); }

float ConVarRefAbstract::GetFloat( CSplitScreenSlot slot ) const
{ return GetAs<float>( slot ); }

int ConVarRefAbstract::GetInt( CSplitScreenSlot slot ) const
{ return GetAs<int>( slot ); }

void ConVarRefAbstract::SetBool( bool value, CSplitScreenSlot slot )
{ SetAs<bool>( value, slot ); }

void ConVarRefAbstract::SetFloat( float value, CSplitScreenSlot slot )
{ SetAs<float>( value, slot ); }

void ConVarRefAbstract::SetInt( int value, CSplitScreenSlot slot )
{ SetAs<int>( value, slot ); }

void ConVarRefAbstract::Clamp( CSplitScreenSlot slot )
{ m_ConVarData->Clamp( slot ); }

CVarTypeTraits * ConVarRefAbstract::TypeTraits() const
{ return m_ConVarData->TypeTraits(); }

ConVarData* ConVarRefAbstract::GetConVarData() const
{ return m_ConVarData; }

bool ConVarRefAbstract::IsConVarDataValid() const
{ return m_ConVarData && GetType() != EConVarType_Invalid && m_ConVarData != TypeTraits()->m_InvalidCvarData; }

bool ConVarRefAbstract::IsConVarDataAvailable() const
{ return !m_ConVarData->IsFlagSet( CVarFlags::REFERENCE ) && IsConVarDataValid(); }

ConVarRefAbstract::ConVarRefAbstract()
: BaseClass(), m_ConVarData( nullptr )
{}

void ConVarRefAbstract::CopyRef( const ConVarRefAbstract &ref )
{
	m_ConVarAccessIndex = ref.m_ConVarAccessIndex;
	m_ConVarRegisteredIndex = ref.m_ConVarRegisteredIndex;
	m_ConVarData = ref.m_ConVarData;
}

CUtlString ConVarRefAbstract::GetString( CSplitScreenSlot slot ) const
{
	return GetAs<CUtlString>( slot );
}
