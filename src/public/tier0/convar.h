#ifndef CONVAR_H
#define CONVAR_H

#if _WIN32
#pragma once
#endif

#include "color.h"
#include "platform.h"
#include "playerslot.h"
#include "public/mathlib/vector2.h"
#include "public/mathlib/vector3.h"
#include "public/mathlib/vector4.h"
#include "public/mathlib/qangle.h"
#include "public/mathlib/vectorws.h"
#include "public/schemasystem/typescope.h"
#include "splitscreenslot.h"
#include "utlcommon.h"
#include "utlstring.h"
#include "utlvector.h"

class CCommandContext;
class CCommand;
class ConVarRefAbstract;

class ConVarUserInfoSet_t
{
public:
	uint8_t *m_pData;
};

enum EConVarType : int16_t
{
	EConVarType_Invalid = -1,
	EConVarType_Bool,
	EConVarType_Int16,
	EConVarType_UInt16,
	EConVarType_Int32,
	EConVarType_UInt32,
	EConVarType_Int64,
	EConVarType_UInt64,
	EConVarType_Float32,
	EConVarType_Float64,
	EConVarType_String,
	EConVarType_Color,
	EConVarType_Vector2,
	EConVarType_Vector3,
	EConVarType_Vector4,
	EConVarType_Qangle,
	EConVarType_VectorWS,
	EConVarType_MAX
};

template<typename T>
constexpr EConVarType TranslateConVarType();

template<> constexpr EConVarType TranslateConVarType<bool>( void )		{ return EConVarType_Bool; }
template<> constexpr EConVarType TranslateConVarType<int16_t>( void )		{ return EConVarType_Int16; }
template<> constexpr EConVarType TranslateConVarType<uint16_t>( void )	{ return EConVarType_UInt16; }
template<> constexpr EConVarType TranslateConVarType<int32_t>( void )		{ return EConVarType_Int32; }
template<> constexpr EConVarType TranslateConVarType<uint32_t>( void )	{ return EConVarType_UInt32; }
template<> constexpr EConVarType TranslateConVarType<int64_t>( void )		{ return EConVarType_Int64; }
template<> constexpr EConVarType TranslateConVarType<uint64_t>( void )	{ return EConVarType_UInt64; }
template<> constexpr EConVarType TranslateConVarType<float>( void )	{ return EConVarType_Float32; }
template<> constexpr EConVarType TranslateConVarType<double>( void )	{ return EConVarType_Float64; }
template<> constexpr EConVarType TranslateConVarType<CUtlString>( void ){ return EConVarType_String; }
template<> constexpr EConVarType TranslateConVarType<Color>( void )		{ return EConVarType_Color; }
template<> constexpr EConVarType TranslateConVarType<Vector2D>( void )	{ return EConVarType_Vector2; }
template<> constexpr EConVarType TranslateConVarType<Vector>( void )	{ return EConVarType_Vector3; }
template<> constexpr EConVarType TranslateConVarType<Vector4D>( void )	{ return EConVarType_Vector4; }
template<> constexpr EConVarType TranslateConVarType<QAngle>( void )	{ return EConVarType_Qangle; }
template<> constexpr EConVarType TranslateConVarType<VectorWS>( void )	{ return EConVarType_VectorWS; }
template<> constexpr EConVarType TranslateConVarType<void*>( void )		{ return EConVarType_Invalid; }

union CVValue_t
{
	static CVValue_t *InvalidValue()
	{
		static CVValue_t s_Data = {};
		return (CVValue_t *)&s_Data;
	}

	CVValue_t() {}
	CVValue_t( const bool value ) : m_bValue( value ) {}
	CVValue_t( const int16_t value ) : m_i16Value( value ) {}
	CVValue_t( const uint16_t value ) : m_u16Value( value ) {}
	CVValue_t( const int32_t value ) : m_i32Value( value ) {}
	CVValue_t( const uint32_t value ) : m_u32Value( value ) {}
	CVValue_t( const int64_t value ) : m_i64Value( value ) {}
	CVValue_t( const uint64_t value ) : m_u64Value( value ) {}
	CVValue_t( const float value ) : m_fl32Value( value ) {}
	CVValue_t( const double value ) : m_fl64Value( value ) {}
	CVValue_t( const CUtlString &value ) : m_StringValue( value ) {}
	CVValue_t( const Color &value ) : m_clrValue( value ) {}
	CVValue_t( const Vector2D &value ) : m_vec2Value( value ) {}
	CVValue_t( const Vector &value ) : m_vec3Value( value ) {}
	CVValue_t( const Vector4D &value ) : m_vec4Value( value ) {}
	CVValue_t( const QAngle &value ) : m_angValue( value ) {}
	CVValue_t( const VectorWS &value ) : m_vecwsValue( value ) {}
	~CVValue_t() {}

	operator bool() const				{ return m_bValue; }
	operator int16_t() const				{ return m_i16Value; }
	operator uint16_t() const				{ return m_u16Value; }
	operator int32_t() const				{ return m_i32Value; }
	operator uint32_t() const				{ return m_u32Value; }
	operator int64_t() const				{ return m_i64Value; }
	operator uint64_t() const				{ return m_u64Value; }
	operator float() const			{ return m_fl32Value; }
	operator double() const			{ return m_fl64Value; }
	operator const char*() const		{ return m_StringValue.Get(); }
	operator const CUtlString&() const	{ return m_StringValue; }
	operator const Color&() const		{ return m_clrValue; }
	operator const Vector2D&() const	{ return m_vec2Value; }
	operator const Vector&() const		{ return m_vec3Value; }
	operator const Vector4D&() const	{ return m_vec4Value; }
	operator const QAngle&() const 		{ return m_angValue; }
	operator const VectorWS&() const 	{ return m_vecwsValue; }

	bool		m_bValue;
	int16_t		m_i16Value;
	uint16_t		m_u16Value;
	int32_t		m_i32Value;
	uint32_t		m_u32Value;
	int64_t		m_i64Value;
	uint64_t		m_u64Value;
	float		m_fl32Value;
	double		m_fl64Value;
	CUtlString	m_StringValue;
	Color		m_clrValue;
	Vector2D	m_vec2Value;
	Vector		m_vec3Value;
	Vector4D	m_vec4Value;
	QAngle		m_angValue;
	VectorWS	m_vecwsValue;
};

struct CVarCreationBase_t
{
	CVarCreationBase_t() :
		m_pszName( nullptr ),
		m_pszHelpString( nullptr ),
		m_nFlags( 0 )
	{}

	const char*				m_pszName;
	const char*				m_pszHelpString;
	uint64_t				m_nFlags;
};

enum CVarFlags : uint64_t
{
    NONE = 0,
    LINKED_CONCOMMAND = (1ull<<0),	
    DEVELOPMENTONLY = (1ull<<1),	
    GAMEDLL = (1ull<<2),	
    CLIENTDLL = (1ull<<3),  
    HIDDEN = (1ull<<4),	
    PROTECTED = (1ull<<5),  
    SPONLY = (1ull<<6),  
    ARCHIVE = (1ull<<7),	
    NOTIFY = (1ull<<8),	
    USERINFO = (1ull<<9),	
    REFERENCE = (1ull<<10), 
    UNLOGGED = (1ull<<11),  
    INITIAL_SETVALUE = (1ull<<12),	
    REPLICATED = (1ull<<13),	
    CHEAT = (1ull<<14), 
    PER_USER = (1ull<<15), 
    DEMO = (1ull<<16), 
    DONTRECORD = (1ull<<17), 
    PERFORMING_CALLBACKS = (1ull<<18),	
    RELEASE = (1ull<<19), 
    MENUBAR_ITEM = (1ull<<20),
    COMMANDLINE_ENFORCED = (1ull<<21), 
    NOT_CONNECTED = (1ull<<22),	
    VCONSOLE_FUZZY_MATCHING = (1ull<<23),
    SERVER_CAN_EXECUTE = (1ull<<24), 
    CLIENT_CAN_EXECUTE = (1ull<<25), 
    SERVER_CANNOT_QUERY = (1ull<<26), 
    VCONSOLE_SET_FOCUS = (1ull<<27),
    CLIENTCMD_CAN_EXECUTE = (1ull<<28),	
    EXECUTE_PER_TICK = (1ull<<29),
    SNAPSHOT_IGNORED = (1ull<<30), 
    DEFENSIVE = (1ull<<32),
    EXECUTE_IMMEDIATELY = (1ull<<33), 
    GAMEINFO_CANNOT_OVERRIDE = (1ull<<34), 
    VCONSOLE_NO_CHANGE_NOTIFY = (1ull<<36),
    ENUM_VALUE = (1ull<<37), 
};

typedef void ( *FnCommandCallback_t )( const CCommandContext &context, const CCommand &command );
typedef void ( *FnCommandCallbackNoContext_t )( const CCommand &command );
typedef void ( *FnCommandCallbackVoid_t )();
typedef void (*FnCommandCompletionCallback)( const CCommand &command, CUtlVector< CUtlString > &completions );

class ICommandCallback
{
public:
	virtual void CommandCallback(const CCommandContext &context, const CCommand &command) = 0;
};

class ICommandCompletionCallback
{
public:
	virtual int  CommandCompletionCallback( const CCommand &command, CUtlVector< CUtlString > &completions ) = 0;
};

enum CommandTarget_t
{
	CT_NO_TARGET = -1,
	CT_FIRST_SPLITSCREEN_CLIENT = 0,
	CT_LAST_SPLITSCREEN_CLIENT = 3,
};

class CCommandContext
{
public:
	CCommandContext( CommandTarget_t nTarget, CPlayerSlot nSlot ) :
		m_nTarget( nTarget ), m_nPlayerSlot( nSlot )
	{}

	CommandTarget_t GetTarget() const
	{
		return m_nTarget;
	}

	CPlayerSlot GetPlayerSlot() const
	{
		return m_nPlayerSlot;
	}

private:
	CommandTarget_t m_nTarget;
	CPlayerSlot m_nPlayerSlot;
};

class CCommand
{
public:
	CCommand();
	CCommand( int nArgC, const char **ppArgV );
	// Tokenizes the other's command string again, so the arguments point into this one's buffers
	CCommand( const CCommand &other ) : CCommand() { *this = other; }
	CCommand &operator=( const CCommand &other );
	bool Tokenize( CUtlString pCommand, characterset_t *pBreakSet = nullptr );
	void Reset();

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[]( int nIndex ) const;	// Gets at arguments
	const char *Arg( int nIndex ) const;		// Gets at arguments
	
	// Helper functions to parse arguments to commands.
	// Returns index of argument, or -1 if no such argument.
	int FindArg( const char *pName ) const;

	int FindArgInt( const char *pName, int nDefaultVal ) const;

	static int MaxCommandLength();
	static characterset_t* DefaultBreakSet();

private:
	void EnsureBuffers();

	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

    int m_nArgv0Size;
    CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgSBuffer;
    CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgvBuffer;
    CUtlVectorFixedGrowable<char*, COMMAND_MAX_ARGC> m_Args;
    // AMNOTE: Gates __beginseq/__endseq handling in the command buffer
    bool m_unk101;
    // Time of the input event the command came from, NaN for other commands
    double m_flInputTime;
    // AMNOTE: Also comes from the input event, 0 for other commands
    uint64_t m_unk201;
};

struct ConCommandCallbackInfo_t
{
	ConCommandCallbackInfo_t() :
		m_fnCommandCallback( nullptr ),
		m_bIsInterface( false ),
		m_bIsVoidCallback( false ),
		m_bIsContextLess( false )
	{}

	ConCommandCallbackInfo_t( FnCommandCallback_t cb ) :
		m_fnCommandCallback( cb ),
		m_bIsInterface( false ),
		m_bIsVoidCallback( false ),
		m_bIsContextLess( false )
	{}

	ConCommandCallbackInfo_t( FnCommandCallbackVoid_t cb ) :
		m_fnVoidCommandCallback( cb ),
		m_bIsInterface( false ),
		m_bIsVoidCallback( cb ? true : false ),
		m_bIsContextLess( false )
	{}

	ConCommandCallbackInfo_t( FnCommandCallbackNoContext_t cb ) :
		m_fnContextlessCommandCallback( cb ),
		m_bIsInterface( false ),
		m_bIsVoidCallback( false ),
		m_bIsContextLess( cb ? true : false )
	{}

	ConCommandCallbackInfo_t( ICommandCallback *cb ) :
		m_pCommandCallback( cb ),
		m_bIsInterface( cb ? true : false ),
		m_bIsVoidCallback( false ),
		m_bIsContextLess( false )
	{}

	bool IsValid() const { return m_fnCommandCallback != nullptr; }

	void Dispatch( const CCommandContext &context, const CCommand &command ) const
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

	union
	{
		FnCommandCallback_t m_fnCommandCallback;
		FnCommandCallbackVoid_t m_fnVoidCommandCallback;
		FnCommandCallbackNoContext_t m_fnContextlessCommandCallback;
		ICommandCallback *m_pCommandCallback;
	};

	bool m_bIsInterface : 1;
	bool m_bIsVoidCallback : 1;
	bool m_bIsContextLess : 1;
};

struct CompletionCallbackInfo_t
{
	CompletionCallbackInfo_t() :
		m_fnCompletionCallback( nullptr ),
		m_bIsInterface( false )
	{}

	CompletionCallbackInfo_t( FnCommandCompletionCallback cb ) :
		m_fnCompletionCallback( cb ),
		m_bIsInterface( false )
	{}

	CompletionCallbackInfo_t( ICommandCompletionCallback *cb ) :
		m_pCommandCompletionCallback( cb ),
		m_bIsInterface( cb ? true : false )
	{}

	bool IsValid() const { return m_fnCompletionCallback != nullptr; }

	int Dispatch( const CCommand &command, CUtlVector< CUtlString > &completions ) const
	{
		if(!IsValid())
			return 0;

		if(m_bIsInterface)
			return m_pCommandCompletionCallback->CommandCompletionCallback( command, completions );
		
		m_fnCompletionCallback( command, completions );
		return completions.Count();
	}

	union
	{
		FnCommandCompletionCallback	m_fnCompletionCallback;
		ICommandCompletionCallback *m_pCommandCompletionCallback;
	};

	bool m_bIsInterface;
};

struct ConCommandCreation_t : CVarCreationBase_t
{
	ConCommandCallbackInfo_t m_CBInfo;
	CompletionCallbackInfo_t m_CompletionCBInfo;
};

class ConCommandData
{
public:
	const char *GetName() const { return m_pszName; }
	const char *GetHelpText() const { return m_pszHelpString; }
	bool HasHelpText() const { return m_pszHelpString && m_pszHelpString[0]; }

	bool IsFlagSet( uint64_t flag ) const { return (m_nFlags & flag) != 0; }
	void AddFlags( uint64_t flags ) { m_nFlags |= flags; }
	void RemoveFlags( uint64_t flags ) { m_nFlags &= ~flags; }
	uint64_t GetFlags( void ) const { return m_nFlags; }

	int GetAutoCompleteSuggestions( const CCommand &command, CUtlVector< CUtlString > &completions ) const
	{
		return m_CompletionCB.Dispatch( command, completions );
	}

	bool HasCompletionCallback() const { return m_CompletionCB.IsValid(); }
	bool HasCallback() const { return m_CallbackInfoIndex > 0; }

	const char*				m_pszName;
	const char*				m_pszHelpString;
	uint64_t				m_nFlags;

	CompletionCallbackInfo_t m_CompletionCB;

	int m_CompletionCBCmdIndex;
	uint16_t m_CallbackInfoIndex;
};

class ConCommandRef
{
private:
	static const uint16_t kInvalidAccessIndex = 0xFFFF;

public:

	ConCommandRef() : m_CommandAccessIndex( kInvalidAccessIndex ), m_CommandRegisteredIndex( 0 ) {}
	ConCommandRef( uint16_t command_idx ) : m_CommandAccessIndex( command_idx ), m_CommandRegisteredIndex( 0 ) {}
	ConCommandRef( uint16_t access_idx, int reg_idx ) : m_CommandAccessIndex( access_idx ), m_CommandRegisteredIndex( reg_idx ) {}

	ConCommandRef( const char *name, bool allow_defensive = false );

	ConCommandData *GetRawData();
	const ConCommandData *GetRawData() const { return const_cast<ConCommandRef *>(this)->GetRawData(); }

	const char *GetName() const { return GetRawData()->GetName(); }
	const char *GetHelpText() const { return GetRawData()->GetHelpText(); }
	bool HasHelpText() const { return GetRawData()->HasHelpText(); }

	bool IsFlagSet( uint64_t flag ) const { return GetRawData()->IsFlagSet( flag ); }
	void AddFlags( uint64_t flags ) { GetRawData()->AddFlags( flags ); }
	void RemoveFlags( uint64_t flags ) { GetRawData()->RemoveFlags( flags ); }
	uint64_t GetFlags() const { return GetRawData()->GetFlags(); }

	bool HasCompletionCallback() const { return GetRawData()->HasCompletionCallback(); }
	bool HasCallback() const { return GetRawData()->HasCallback(); }

	void Dispatch( const CCommandContext &context, const CCommand &command );

	int GetCompletionCommandIndex() const { return GetRawData()->m_CompletionCBCmdIndex; }
	uint16_t GetCallbackIndex() const { return GetRawData()->m_CallbackInfoIndex; }

	int GetAutoCompleteSuggestions( const CCommand &command, CUtlVector< CUtlString > &completions ) const
	{
		return GetRawData()->GetAutoCompleteSuggestions( command, completions );
	}

	void InvalidateRef() { m_CommandAccessIndex = kInvalidAccessIndex; m_CommandRegisteredIndex = 0; }
	bool IsValidRef() const { return m_CommandAccessIndex != kInvalidAccessIndex; }
	uint16_t GetAccessIndex() const { return m_CommandAccessIndex; }
	int GetRegisteredIndex() const { return m_CommandRegisteredIndex; }

private:
	uint16_t m_CommandAccessIndex;
	int m_CommandRegisteredIndex;
};

class ConCommand : public ConCommandRef
{
public:
	typedef ConCommandRef BaseClass;

	ConCommand( const char *pName, ConCommandCallbackInfo_t callback,
		const char *pHelpString, uint64_t flags = 0,
		CompletionCallbackInfo_t completionFunc = CompletionCallbackInfo_t() )
		: BaseClass()
	{
		Create( pName, callback, pHelpString, flags, completionFunc );
	}

	~ConCommand()
	{
		Destroy();
	}

protected:
	void Create( const char *pName, const ConCommandCallbackInfo_t &cb, const char *pHelpString, uint64_t flags, const CompletionCallbackInfo_t &completion_cb );
	void Destroy( );
};

/////////////////////////////////////////////
// convars
////////////////////////////////////////////

template <typename T>
class CConVar;

template <typename T>
using FnTypedChangeCallback_t = void(*)(CConVar<T> *cvar, CSplitScreenSlot nSlot, const T *pNewValue, const T *pOldValue);
template <typename T>
using FnTypedChangeCallbackProvider_t = void(*)(CConVar<T> *cvar, CSplitScreenSlot slot, const T *pNewValue, const T *pOldValue, ConVarUserInfoSet_t *userinfo_data, FnTypedChangeCallback_t<T> cb);

using FnGenericChangeCallback_t = void(*)(ConVarRefAbstract *ref, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue);
using FnGenericChangeCallbackProvider_t = void(*)(ConVarRefAbstract *ref, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue, ConVarUserInfoSet_t *userinfo_data, FnGenericChangeCallback_t cb);

template <typename T>
using FnTypedFilterCallback_t = bool(*)(CConVar<T> *cvar, CSplitScreenSlot nSlot, const T *pNewValue, const T *pOldValue);
template <typename T>
using FnTypedFilterCallbackProvider_t = bool(*)(CConVar<T> *cvar, CSplitScreenSlot slot, const T *pNewValue, const T *pOldValue, ConVarUserInfoSet_t *userinfo_data, FnTypedFilterCallback_t<T> cb);

using FnGenericFilterCallback_t = bool(*)(ConVarRefAbstract *ref, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue);
using FnGenericFilterCallbackProvider_t = bool(*)(ConVarRefAbstract *ref, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue, ConVarUserInfoSet_t *userinfo_data, FnGenericFilterCallback_t cb);

using FnGetEnumInfoHandle_t = SchemaMetaInfoHandle_t<CSchemaEnumInfo> (*)();

#pragma pack(push,1)
struct ConVarValueInfo_t
{
	ConVarValueInfo_t( EConVarType type = EConVarType_Invalid ) :
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

	template <typename T>
	void SetDefaultValue( const T &value )
	{
		m_bHasDefault = true;
		*reinterpret_cast<T *>(m_defaultValue) = value;
	}

	template <typename T>
	void SetMinValue( const T &min )
	{
		m_bHasMin = true;
		*reinterpret_cast<T *>(m_minValue) = min;
	}

	template <typename T>
	void SetMaxValue( const T &max )
	{
		m_bHasMax = true;
		*reinterpret_cast<T *>(m_maxValue) = max;
	}

	template <typename T>
	void SetCallback( FnTypedChangeCallback_t<T> cb )
	{
		if(cb)
		{
			m_fnProviderCallBack = []( ConVarRefAbstract *ref, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue, ConVarUserInfoSet_t *userinfo_data, FnGenericChangeCallback_t cb ) {
				reinterpret_cast<FnTypedChangeCallback_t<T>>(cb)(reinterpret_cast<CConVar<T> *>(ref), nSlot, reinterpret_cast<const T *>(pNewValue), reinterpret_cast<const T *>(pOldValue));
			};

			m_fnCallBack = reinterpret_cast<FnGenericChangeCallback_t>(cb);
		}
	}

	template <typename T>
	void SetFilterCallback( FnTypedFilterCallback_t<T> cb )
	{
		if(cb)
		{
			m_fnProviderFilterCallBack = []( ConVarRefAbstract *ref, CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue, ConVarUserInfoSet_t *userinfo_data, FnGenericFilterCallback_t cb ) {
				return reinterpret_cast<FnTypedFilterCallback_t<T>>(cb)(reinterpret_cast<CConVar<T> *>(ref), nSlot, reinterpret_cast<const T *>(pNewValue), reinterpret_cast<const T *>(pOldValue));
			};

			m_fnFilterCallBack = reinterpret_cast<FnGenericFilterCallback_t>(cb);
		}
	}

	void SetCompletionCallback( const CompletionCallbackInfo_t &cb )
	{
		m_CompletionCallBack = cb;
	}

	int32_t m_Version;

	bool m_bHasDefault;
	bool m_bHasMin;
	bool m_bHasMax;

private:
	CVValue_t m_defaultValue = {};
	CVValue_t m_minValue = {};
	CVValue_t m_maxValue = {};

public:
	FnGenericChangeCallbackProvider_t m_fnProviderCallBack;
	FnGenericChangeCallback_t m_fnCallBack;

	FnGenericFilterCallbackProvider_t m_fnProviderFilterCallBack;
	FnGenericFilterCallback_t m_fnFilterCallBack;

	FnGetEnumInfoHandle_t m_fnGetEnumInfoHandle;

	EConVarType m_eVarType;

	CompletionCallbackInfo_t m_CompletionCallBack;
};
#pragma pack(pop)

struct ConVarCreation_t : CVarCreationBase_t
{
	ConVarValueInfo_t m_valueInfo;
};

template <typename T> static void CvarTypeTrait_ConstructFn( CVValue_t *obj ) { Construct( (T *)obj ); }
template <typename T> static void CvarTypeTrait_CopyFn( CVValue_t *obj, const CVValue_t &other ) { *(T *)obj = *(T *)&other; }
template <typename T> static void CvarTypeTrait_DestructFn( CVValue_t *obj ) { Destruct( (T *)obj ); }
template <typename T> static bool CvarTypeTrait_StringToValueFn( const char *string, CVValue_t *obj ) { return V_StringToValue<T>( string, *(T *)obj ); }
template <typename T> static void CvarTypeTrait_ValueToStringFn( const CVValue_t *obj, CBufferString &buf );
template <typename T> static bool CvarTypeTrait_EqualFn( const CVValue_t *obj, const CVValue_t *other ) { return *(T *)obj == *(T *)other; }
template <typename T> static void CvarTypeTrait_ClampFn( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min)
		*(T *)obj = Max( *(T *)obj, *(T *)min );

	if(max)
		*(T *)obj = Min( *(T *)obj, *(T *)max );
}

template<> bool CvarTypeTrait_StringToValueFn<CUtlString>( const char *string, CVValue_t *obj ) { obj->m_StringValue = string; return true; }

template<> void CvarTypeTrait_ValueToStringFn<bool>( const CVValue_t *obj, CBufferString &buf ) { buf.Insert( 0, obj->m_bValue ? "true" : "false" ); }
template<> void CvarTypeTrait_ValueToStringFn<int16_t>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%d", obj->m_i16Value ); }
template<> void CvarTypeTrait_ValueToStringFn<uint16_t>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%u", obj->m_u16Value ); }
template<> void CvarTypeTrait_ValueToStringFn<int32_t>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%d", obj->m_i32Value ); }
template<> void CvarTypeTrait_ValueToStringFn<uint32_t>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%u", obj->m_u32Value ); }
template<> void CvarTypeTrait_ValueToStringFn<int64_t>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%lld", obj->m_i64Value ); }
template<> void CvarTypeTrait_ValueToStringFn<uint64_t>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%llu", obj->m_u64Value ); }
template<> void CvarTypeTrait_ValueToStringFn<float>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%f", obj->m_fl32Value ); }
template<> void CvarTypeTrait_ValueToStringFn<double>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%lf", obj->m_fl64Value ); }
template<> void CvarTypeTrait_ValueToStringFn<CUtlString>( const CVValue_t *obj, CBufferString &buf ) { buf.Insert( 0, obj->m_StringValue.Get() ); }
template<> void CvarTypeTrait_ValueToStringFn<Vector2D>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%f %f", obj->m_vec2Value[0], obj->m_vec2Value[1] ); }
template<> void CvarTypeTrait_ValueToStringFn<Vector>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%f %f %f", obj->m_vec3Value[0], obj->m_vec3Value[1], obj->m_vec3Value[2] ); }
template<> void CvarTypeTrait_ValueToStringFn<Vector4D>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%f %f %f %f", obj->m_vec4Value[0], obj->m_vec4Value[1], obj->m_vec4Value[2], obj->m_vec4Value[3] ); }
template<> void CvarTypeTrait_ValueToStringFn<QAngle>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%f %f %f", obj->m_angValue[0], obj->m_angValue[1], obj->m_angValue[2] ); }
template<> void CvarTypeTrait_ValueToStringFn<VectorWS>( const CVValue_t *obj, CBufferString &buf ) { buf.Format( "%f %f %f", obj->m_vecwsValue[0], obj->m_vecwsValue[1], obj->m_vecwsValue[2] ); }
template<> void CvarTypeTrait_ValueToStringFn<Color>( const CVValue_t *obj, CBufferString &buf )
{
	if( obj->m_clrValue.a() == 255 )
		buf.Format( "%d %d %d", obj->m_clrValue[0], obj->m_clrValue[1], obj->m_clrValue[2] );
	else
		buf.Format( "%d %d %d %d", obj->m_clrValue[0], obj->m_clrValue[1], obj->m_clrValue[2], obj->m_clrValue[3] );
}

template<> void CvarTypeTrait_ClampFn<bool>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max ) { }
template<> void CvarTypeTrait_ClampFn<CUtlString>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max ) { }
template<> void CvarTypeTrait_ClampFn<Color>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min) for(int i = 0; i < 4; i++) obj->m_clrValue[i] = (std::max)( obj->m_clrValue[i], min->m_clrValue[i] );
	if(max) for(int i = 0; i < 4; i++) obj->m_clrValue[i] = (std::min)( obj->m_clrValue[i], max->m_clrValue[i] );
}
template<> void CvarTypeTrait_ClampFn<Vector2D>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min) for(int i = 0; i < 2; i++) obj->m_vec2Value[i] = (std::max)( obj->m_vec2Value[i], min->m_vec2Value[i] );
	if(max) for(int i = 0; i < 2; i++) obj->m_vec2Value[i] = (std::min)( obj->m_vec2Value[i], max->m_vec2Value[i] );
}
template<> void CvarTypeTrait_ClampFn<Vector>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min) for(int i = 0; i < 3; i++) obj->m_vec3Value[i] = (std::max)( obj->m_vec3Value[i], min->m_vec3Value[i] );
	if(max) for(int i = 0; i < 3; i++) obj->m_vec3Value[i] = (std::min)( obj->m_vec3Value[i], max->m_vec3Value[i] );
}
template<> void CvarTypeTrait_ClampFn<Vector4D>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min) for(int i = 0; i < 4; i++) obj->m_vec4Value[i] = (std::max)( obj->m_vec4Value[i], min->m_vec4Value[i] );
	if(max) for(int i = 0; i < 4; i++) obj->m_vec4Value[i] = (std::min)( obj->m_vec4Value[i], max->m_vec4Value[i] );
}
template<> void CvarTypeTrait_ClampFn<QAngle>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min) for(int i = 0; i < 3; i++) obj->m_angValue[i] = (std::max)( obj->m_angValue[i], min->m_angValue[i] );
	if(max) for(int i = 0; i < 3; i++) obj->m_angValue[i] = (std::min)( obj->m_angValue[i], max->m_angValue[i] );
}
template<> void CvarTypeTrait_ClampFn<VectorWS>( CVValue_t *obj, const CVValue_t *min, const CVValue_t *max )
{
	if(min) for(int i = 0; i < 3; i++) obj->m_vecwsValue[i] = (std::max)( obj->m_vecwsValue[i], min->m_vecwsValue[i] );
	if(max) for(int i = 0; i < 3; i++) obj->m_vecwsValue[i] = (std::min)( obj->m_vecwsValue[i], max->m_vecwsValue[i] );
}

class ConVarData;
ConVarData *GetInvalidConVarData( EConVarType type );

struct CVarTypeTraits
{
	template <typename T>
	CVarTypeTraits *InitAs( const char *name, const char *default_value )
	{
		m_TypeName = name;
		m_ByteSize = sizeof( T );
		m_IsPrimitive = CTypePOD<T>;

		Construct = &CvarTypeTrait_ConstructFn<T>;
		Copy = &CvarTypeTrait_CopyFn<T>;
		Destruct = &CvarTypeTrait_DestructFn<T>;
		StringToValue = &CvarTypeTrait_StringToValueFn<T>;
		ValueToString = &CvarTypeTrait_ValueToStringFn<T>;
		Equal = &CvarTypeTrait_EqualFn<T>;
		Clamp = &CvarTypeTrait_ClampFn<T>;

		m_DefaultValue = default_value;
		m_InvalidCvarData = GetInvalidConVarData( TranslateConVarType<T>() );

		return this;
	}

	const char *m_TypeName;
	int m_ByteSize;
	bool m_IsPrimitive;

	void (*Construct)(CVValue_t *obj);
	void (*Copy)(CVValue_t *obj, const CVValue_t &other);
	void (*Destruct)(CVValue_t *obj);
	bool (*StringToValue)(const char *string, CVValue_t *obj);
	void (*ValueToString)(const CVValue_t *obj, CBufferString &buf);
	bool (*Equal)(const CVValue_t *obj, const CVValue_t *other);
	void (*Clamp)(CVValue_t *obj, const CVValue_t *min, const CVValue_t *max);

	const char *m_DefaultValue;
	ConVarData *m_InvalidCvarData;
};

CVarTypeTraits *GetCvarTypeTraits( EConVarType type );

#pragma pack(push,1)
class ConVarData
{
public:
	enum
	{
		// GameInfo was used to initialize this cvar
		CVARGI_INITIALIZED = (1 << 0),
		// GameInfo has set default value, and it cannot be overriden
		CVARGI_HAS_DEFAULT_VALUE = (1 << 1),
		// GameInfo has set min value, and it cannot be overriden
		CVARGI_HAS_MIN_VALUE = (1 << 2),
		// GameInfo has set max value, and it cannot be overriden
		CVARGI_HAS_MAX_VALUE = (1 << 3),
		// GameInfo has set cvar version
		CVARGI_HAS_VERSION = (1 << 4)
	};

	ConVarData( EConVarType type = EConVarType_Invalid ): m_Value {}
	{
		Invalidate( type, true );
	}

	// Helper method to invalidate convar data to its default, pre-register state
	void Invalidate( EConVarType type = EConVarType_Invalid, bool as_undefined = false )
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

	const char *GetName( void ) const { return m_pszName; }
	const char *GetHelpText( void ) const { return m_pszHelpString; }
	bool HasHelpText() const { return m_pszHelpString && m_pszHelpString[0]; }

	EConVarType	GetType() const { return m_eVarType; }

	short GetVersion() const { return m_Version; }

	int	GetTimesChanged() const { return m_iTimesChanged; }
	void SetTimesChanged( int val ) { m_iTimesChanged = val; }
	void IncrementTimesChanged() { m_iTimesChanged++; }

	bool IsFlagSet( uint64_t flag ) const { return (m_nFlags & flag) != 0; }
	void AddFlags( uint64_t flags ) { m_nFlags |= flags; }
	void RemoveFlags( uint64_t flags ) { m_nFlags &= ~flags; }
	uint64_t GetFlags() const { return m_nFlags; }

	int GetGameInfoFlags() const { return m_GameInfoFlags; }
	int GetCallbackIndex() const { return m_iCallbackIndex; }
	int GetUserInfoByteIndex() const { return m_UserInfoByteIndex; }

	int GetMaxSplitScreenSlots() const;

	CVarTypeTraits *TypeTraits() const
	{
		Assert( m_eVarType != EConVarType_Invalid );
		return GetCvarTypeTraits( m_eVarType );
	}

	bool HasEnumInfo() const { return m_fnGetEnumInfoHandle != nullptr; }
	SchemaMetaInfoHandle_t<CSchemaEnumInfo> GetEnumInfo() const { return HasEnumInfo() ? m_fnGetEnumInfoHandle() : SchemaMetaInfoHandle_t<CSchemaEnumInfo> {}; }

	int GetDataByteSize() const { return TypeTraits()->m_ByteSize; }
	bool IsPrimitiveType() const { return TypeTraits()->m_IsPrimitive; }
	const char *GetDataTypeName() const { return TypeTraits()->m_TypeName; }

	bool HasDefaultValue() const { return m_defaultValue && m_defaultValue != CVValue_t::InvalidValue(); }
	bool HasMinValue() const { return m_minValue != nullptr; }
	bool HasMaxValue() const { return m_maxValue != nullptr; }

	CVValue_t *DefaultValue() const { return m_defaultValue; }
	CVValue_t *MinValue() const { return m_minValue; }
	CVValue_t *MaxValue() const { return m_maxValue; }

	// Since cvars can't be without default value, this just resets it to a global default value if it had one set
	void RemoveDefaultValue() { if(HasDefaultValue()) TypeTraits()->StringToValue( TypeTraits()->m_DefaultValue, m_defaultValue ); }

	// AMNOTE: Min/Max values are allocated by default by a CCvar allocator which doesn't cleanup memory,
	// thus be careful when removing min/max values allocated by other sources and do cleanup yourself
	void RemoveMinValue() { m_minValue = nullptr; }
	void RemoveMaxValue() { m_maxValue = nullptr; }

	// Updates default/min/max values to other value if present
	bool UpdateDefaultValueString( const char *value ) { return HasDefaultValue() && TypeTraits()->StringToValue( value, m_defaultValue ); }
	bool UpdateMinValueString( const char *value ) { return HasMinValue() && TypeTraits()->StringToValue( value, m_minValue ); }
	bool UpdateMaxValueString( const char *value ) { return HasMaxValue() && TypeTraits()->StringToValue( value, m_maxValue ); }

	// Sets default value by copying it, doesn't need to be allocated
	void SetDefaultValue( CVValue_t *value ) { TypeTraits()->Copy( m_defaultValue, *value ); }

	// AMNOTE: Expects you to manually allocate its value and for it to be alive while it's used by the cvar
	// Also you should be responsible for clearing memory on cleanup, by default game uses CCvar memory allocator for this
	// These are ignored for string and bool types!
	void SetMinValue( CVValue_t *value ) { m_minValue = value; }
	void SetMaxValue( CVValue_t *value ) { m_maxValue = value; }

	// Could be nullptr if slot is invalid for this cvar
	CVValue_t *Value( CSplitScreenSlot slot ) const;
	// Returns default_value if slot is invalid
	CVValue_t *ValueOrDefault( CSplitScreenSlot slot ) const;

	bool IsSetToDefault( CSplitScreenSlot slot ) const { return TypeTraits()->Equal( ValueOrDefault( slot ), m_defaultValue ); }
	bool IsAllSetToDefault() const;

	void ValueToString( CSplitScreenSlot slot, CBufferString &buf ) const { TypeTraits()->ValueToString( ValueOrDefault( slot ), buf ); }
	void DefaultValueToString( CBufferString &buf ) const { TypeTraits()->ValueToString( m_defaultValue, buf ); }
	void MinValueToString( CBufferString &buf ) const;
	void MaxValueToString( CBufferString &buf ) const;

	void Construct( CSplitScreenSlot slot ) const { if(IsSlotInRange( slot )) TypeTraits()->Construct( Value( slot ) ); }
	void Destruct( CSplitScreenSlot slot ) const { if(IsSlotInRange( slot )) TypeTraits()->Destruct( Value( slot ) ); }
	bool IsEqual( CSplitScreenSlot slot, CVValue_t *other ) const { return TypeTraits()->Equal( ValueOrDefault( slot ), other ); }
	void Clamp( CSplitScreenSlot slot ) const { if(IsSlotInRange( slot )) TypeTraits()->Clamp( Value( slot ), m_minValue, m_maxValue ); }

private:
	bool IsSlotInRange( CSplitScreenSlot slot ) const { return slot.Get() == -1 || (slot.Get() >= 0 && slot.Get() < GetMaxSplitScreenSlots()); }

	const char* m_pszName;

	CVValue_t* m_defaultValue;

	CVValue_t* m_minValue;
	CVValue_t* m_maxValue;

	const char* m_pszHelpString;
	EConVarType m_eVarType;

	short m_Version;

	unsigned int m_iTimesChanged;
	uint64_t m_nFlags;

	unsigned int m_iCallbackIndex;
	unsigned int m_iFilterCBIndex;
	unsigned int m_iCompletionCBIndex;

	int m_GameInfoFlags;
	int m_UserInfoByteIndex;

	FnGetEnumInfoHandle_t m_fnGetEnumInfoHandle;

	CVValue_t m_Value;
};
#pragma pack(pop)

class ConVarRef
{
private:
	static const uint16_t kInvalidAccessIndex = 0xFFFF;

public:
	ConVarRef() : m_ConVarAccessIndex( kInvalidAccessIndex ), m_ConVarRegisteredIndex( 0 ) {}
	ConVarRef( uint16_t convar_idx ) : m_ConVarAccessIndex( convar_idx ), m_ConVarRegisteredIndex( 0 ) {}
	ConVarRef( uint16_t convar_idx, int registered_idx ) : m_ConVarAccessIndex( convar_idx ), m_ConVarRegisteredIndex( registered_idx ) {}

	ConVarRef( const char *name, bool allow_defensive = false );

	void InvalidateRef() { m_ConVarAccessIndex = kInvalidAccessIndex; m_ConVarRegisteredIndex = 0; }
	bool IsValidRef() const { return m_ConVarAccessIndex != kInvalidAccessIndex; }
	uint16_t GetAccessIndex() const { return m_ConVarAccessIndex; }
	int GetRegisteredIndex() const { return m_ConVarRegisteredIndex; }

protected:
	// Index into internal linked list of concommands
	uint16_t m_ConVarAccessIndex;
	// ConVar registered positional index
	int m_ConVarRegisteredIndex;
};

class ConVarRefAbstract : public ConVarRef
{
public:
	typedef ConVarRef BaseClass;

	ConVarRefAbstract( const char *name, bool allow_defensive = false )
		: BaseClass( name, allow_defensive ), m_ConVarData( nullptr )
	{
		Init( *this );
	}

	ConVarRefAbstract( ConVarRef ref, EConVarType type = EConVarType_Invalid )
		: BaseClass( ref ), m_ConVarData( nullptr )
	{
		Init( *this, type );
	}

	ConVarRefAbstract( const ConVarRefAbstract &ref )
		: BaseClass(), m_ConVarData( nullptr )
	{
		CopyRef( ref );
	}

	const char *GetName() const { return m_ConVarData->GetName(); }
	const char *GetHelpText() const { return m_ConVarData->GetHelpText(); }
	bool HasHelpText() const { return m_ConVarData->HasHelpText(); }

	EConVarType	GetType() const { return m_ConVarData->GetType(); }

	bool HasDefault() const { return m_ConVarData->HasDefaultValue(); }
	bool HasMin() const { return m_ConVarData->HasMinValue(); }
	bool HasMax() const { return m_ConVarData->HasMaxValue(); }

	bool IsFlagSet( uint64_t flag ) const { return m_ConVarData->IsFlagSet( flag ); }
	void AddFlags( uint64_t flags ) { m_ConVarData->AddFlags( flags ); }
	void RemoveFlags( uint64_t flags ) { return m_ConVarData->RemoveFlags( flags ); }
	uint64_t GetFlags( void ) const { return m_ConVarData->GetFlags(); }

	// Returns true if convar should be treated as hidden
	bool ShouldBeHidden() const { return !m_ConVarData || IsFlagSet( CVarFlags::REFERENCE | CVarFlags::HIDDEN | CVarFlags::DEVELOPMENTONLY ); }

	bool IsSetToDefault( CSplitScreenSlot slot = -1 ) const { return m_ConVarData->IsSetToDefault( slot ); }
	bool IsAllSetToDefault() const { return m_ConVarData->IsAllSetToDefault(); }

	void GetValueAsString( CBufferString &buf, CSplitScreenSlot slot = -1 ) const { m_ConVarData->ValueToString( slot, buf ); }
	void GetDefaultAsString( CBufferString &buf ) const { m_ConVarData->DefaultValueToString( buf ); }
	void GetMinAsString( CBufferString &buf ) const { m_ConVarData->MinValueToString( buf ); }
	void GetMaxAsString( CBufferString &buf ) const { m_ConVarData->MaxValueToString( buf ); }

	bool IsValuePrimitive() const { return m_ConVarData->GetType() != EConVarType_Invalid && m_ConVarData->IsPrimitiveType(); }

	// Attempts to get value as a type T, does type conversion if possible,
	// if no such action is available for CvarType/T combo, global default value for type T would be returned
	template <typename T>
	const T GetAs( CSplitScreenSlot slot = -1 ) const;

	// Attempts to get value as a bool, does type conversion if possible,
	// if no such action is available for CvarType/bool combo, global default value for bool would be returned
	bool GetBool( CSplitScreenSlot slot = -1 ) const { return GetAs<bool>( slot ); }
	// Attempts to get value as a float, does type conversion if possible,
	// if no such action is available for CvarType/float combo, global default value for float would be returned
	float GetFloat( CSplitScreenSlot slot = -1 ) const { return GetAs<float>( slot ); }
	// Attempts to get value as an int, does type conversion if possible,
	// if no such action is available for CvarType/int combo, global default value for int would be returned
	int GetInt( CSplitScreenSlot slot = -1 ) const { return GetAs<int>( slot ); }
	// Parses the value to string, mostly the same to GetValueAsString
	CUtlString GetString( CSplitScreenSlot slot = -1 ) const;

	// Attempts to set value as a type T, does type conversion if possible,
	// if no such action is available for CvarType/T combo, no action would be done
	// 
	// AMNOTE: Using this function (as well as SetBool, SetFloat and SetInt) in a
	// change callbacks (either global or cvar one) will result in a crash since it
	// does constructor casts to CConVarRef<T> which would be invalid if setvalue is queued
	// (which will happen in callbacks), so the solution is to do a static_cast to
	// CConVarRef<T> yourself on the cvar you receive in a callback and call .Set() on it
	template <typename T>
	void SetAs( const T &value, CSplitScreenSlot slot = -1 );

	// Attempts to set value as a bool, does type conversion if possible,
	// if no such action is available for CvarType/bool combo, no action would be done
	void SetBool( bool value, CSplitScreenSlot slot = -1 ) { SetAs<bool>( value, slot ); }
	// Attempts to set value as a float, does type conversion if possible,
	// if no such action is available for CvarType/float combo, no action would be done
	void SetFloat( float value, CSplitScreenSlot slot = -1 ) { SetAs<float>( value, slot ); }
	// Attempts to set value as an int, does type conversion if possible,
	// if no such action is available for CvarType/int combo, no action would be done
	void SetInt( int value, CSplitScreenSlot slot = -1 ) { SetAs<int>( value, slot ); }
	// Parses the string to CvarType type, returns true on success, false otherwise
	bool SetString( CUtlString string, CSplitScreenSlot slot = -1 );

	// Reset to default value
	void Revert( CSplitScreenSlot slot = -1 );
	// Clamps value to min/max bounds if set
	void Clamp( CSplitScreenSlot slot = -1 ) { m_ConVarData->Clamp( slot ); }

	CVarTypeTraits *TypeTraits() const { return m_ConVarData->TypeTraits(); }
	ConVarData* GetConVarData() const { return m_ConVarData; };

	// Checks if stored ConVarData points to invalid convar data
	bool IsConVarDataValid() const { return m_ConVarData && GetType() != EConVarType_Invalid && m_ConVarData != TypeTraits()->m_InvalidCvarData; }

	// Checks if ConVarData is available for usage (means cvar was registered),
	// mostly useful for CConVarRef cvars which register themselves partially
	bool IsConVarDataAvailable() const { return !m_ConVarData->IsFlagSet( CVarFlags::REFERENCE ) && IsConVarDataValid(); }

protected:
	ConVarRefAbstract() : BaseClass(), m_ConVarData( nullptr ) {}

	void CopyRef( const ConVarRefAbstract &ref )
	{
		m_ConVarAccessIndex = ref.m_ConVarAccessIndex;
		m_ConVarRegisteredIndex = ref.m_ConVarRegisteredIndex;
		m_ConVarData = ref.m_ConVarData;
	}

	void CallChangeCallbacks( CSplitScreenSlot slot, CVValue_t *new_value, CVValue_t *prev_value, const char *new_str, const char *prev_str );

	void SetOrQueueValueInternal( CSplitScreenSlot slot, CVValue_t *value );
	void QueueSetValueInternal( CSplitScreenSlot slot, CVValue_t *value );
	void SetValueInternal( CSplitScreenSlot slot, CVValue_t *value );

	// Does type conversion from CvarType to type T, only valid for primitive types
	template <typename T>
	T ConvertFromPrimitiveTo( CSplitScreenSlot slot ) const;
	// Does type conversion from type T to CvarType, only valid for primitive types
	template <typename T>
	void ConvertToPrimitiveFrom( CSplitScreenSlot slot, const T &value ) const;

	// Initialises this cvar, if ref is invalid, ConVarData would be initialised to invalid convar data of a set type
	void Init( ConVarRef ref, EConVarType type = EConVarType_Invalid );

	void InvalidateConVarData( EConVarType type = EConVarType_Invalid );

	ConVarData* m_ConVarData;
};

uint64_t SanitiseConVarFlags( uint64_t flags );
void SetupConVar( ConVarRefAbstract *cvar, ConVarData **cvar_data, ConVarCreation_t &info );
void UnRegisterConVar( ConVarRef *cvar );

template<typename T>
class CConVarRef : public ConVarRefAbstract
{
public:
	typedef ConVarRefAbstract BaseClass;

	// Creates a convar ref that will pre-register convar in a system if it's not yet registered
	// otherwise it just acts as a normal reference to a convar
	// Mostly useful for cross module convar access, where you can't guarantee the order of creation
	// and can't get direct access to convar CConVar<T>
	CConVarRef( const char *name ) : BaseClass()
	{
		ConVarValueInfo_t value_info( TranslateConVarType<T>() );
		Register( name, CVarFlags::REFERENCE, nullptr, value_info );
	}

	// Constructs typed cvar ref if the type matches, otherwise this would be initialised to invalid convar data!
	CConVarRef( const ConVarRefAbstract &ref ) : BaseClass()
	{
		// If the ref type doesn't match ours, bad cast was attempted,
		// fall back to invalid cvar data
		if(ref.GetType() == TranslateConVarType<T>())
			CopyRef( ref );
		else
			Init( ConVarRef(), TranslateConVarType<T>() );
	}

	// Constructs typed cvar ref if the type matches, otherwise this would be initialised to invalid convar data!
	CConVarRef( const ConVarRef &ref ) : CConVarRef( ConVarRefAbstract( ref ) ) { }

	const T &Get( CSplitScreenSlot slot = -1 ) const { return *reinterpret_cast<T *>(m_ConVarData->ValueOrDefault( slot )); }
	void Set( const T &value, CSplitScreenSlot slot = -1 );

	const T &GetDefault() const { return *reinterpret_cast<T *>(m_ConVarData->DefaultValue()); }
	const T &GetMin() const { return *reinterpret_cast<T *>(m_ConVarData->MinValue()); }
	const T &GetMax() const { return *reinterpret_cast<T *>(m_ConVarData->MaxValue()); }

protected:
	CConVarRef() : BaseClass() {}

	void Register( const char *name, uint64_t flags, const char *help_string, const ConVarValueInfo_t &value_info )
	{
		Assert( name );

		Init( ConVarRef(), TranslateConVarType<T>() );

		ConVarCreation_t info;
		info.m_pszName = name;
		info.m_pszHelpString = help_string;
		info.m_nFlags = SanitiseConVarFlags( flags );
		info.m_valueInfo = value_info;

		SetupConVar( this, &m_ConVarData, info );
	}
};

template<typename T>
class CConVar : public CConVarRef<T>
{
public:
	typedef CConVarRef<T> BaseClass;

	CConVar( const char *name, uint64_t flags, const char *help_string, const T &default_value, FnTypedChangeCallback_t<T> cb = nullptr, const CompletionCallbackInfo_t &completion_cb = {} )
		: BaseClass()
	{
		Assert( name );

		BaseClass::Init( ConVarRef(), TranslateConVarType<T>() );

		ConVarValueInfo_t value_info( TranslateConVarType<T>() );
		value_info.SetDefaultValue( default_value );
		value_info.SetCallback( cb );

		if(completion_cb.IsValid())
		{
			value_info.SetCompletionCallback( completion_cb );
			flags |= CVarFlags::VCONSOLE_FUZZY_MATCHING;
		}

		BaseClass::Register( name, flags, help_string, value_info );
	}

	CConVar( const char *name, uint64_t flags, const char *help_string, const T &default_value, bool min, const T &minValue, bool max, const T &maxValue, FnTypedChangeCallback_t<T> cb = nullptr, FnTypedFilterCallback_t<T> filter_cb = nullptr, const CompletionCallbackInfo_t &completion_cb = {} )
		: BaseClass()
	{
		Assert( name );

		BaseClass::Init( ConVarRef(), TranslateConVarType<T>() );

		ConVarValueInfo_t value_info( TranslateConVarType<T>() );
		value_info.SetDefaultValue( default_value );

		if(min)
			value_info.SetMinValue( minValue );

		if(max)
			value_info.SetMaxValue( maxValue );

		value_info.SetCallback( cb );
		value_info.SetFilterCallback( filter_cb );
		
		if(completion_cb.IsValid())
		{
			value_info.SetCompletionCallback( completion_cb );
			flags |= CVarFlags::VCONSOLE_FUZZY_MATCHING;
		}

		BaseClass::Register( name, flags, help_string, value_info );
	}

	~CConVar()
	{
		UnRegisterConVar( this );
		BaseClass::InvalidateConVarData();
	}
};

template<typename T>
inline const T ConVarRefAbstract::GetAs( CSplitScreenSlot slot ) const
{
	CVValue_t *value = m_ConVarData->ValueOrDefault( slot );

	if(GetType() == TranslateConVarType<T>())
		return *value;
	else if(GetType() == EConVarType_String)
	{
		CVValue_t obj;
		GetCvarTypeTraits( TranslateConVarType<T>() )->Construct( &obj );

		if(GetCvarTypeTraits( TranslateConVarType<T>() )->StringToValue( value->m_StringValue.Get(), &obj ))
		{
			T ret = obj;
			GetCvarTypeTraits( TranslateConVarType<T>() )->Destruct( &obj );
			return ret;
		}

		GetCvarTypeTraits( TranslateConVarType<T>() )->Destruct( &obj );
	}
	else if(IsValuePrimitive())
		return ConvertFromPrimitiveTo<T>( slot );

	return *GetInvalidConVarData( TranslateConVarType<T>() )->DefaultValue();
}

template<>
inline const CUtlString ConVarRefAbstract::GetAs( CSplitScreenSlot slot ) const
{
	CBufferString buf;
	GetValueAsString( buf, slot );
	return buf.Get();
}

inline CUtlString ConVarRefAbstract::GetString( CSplitScreenSlot slot ) const
{
	return GetAs<CUtlString>( slot );
}

template<typename T>
inline T ConVarRefAbstract::ConvertFromPrimitiveTo( CSplitScreenSlot slot ) const
{
	if constexpr(CTypePOD<T>)
	{
		CVValue_t *value = m_ConVarData->ValueOrDefault( slot );

		switch(GetType())
		{
			case EConVarType_Bool:		return value->m_bValue;
			case EConVarType_Int16:		return value->m_i16Value;
			case EConVarType_UInt16:	return value->m_u16Value;
			case EConVarType_Int32:		return value->m_i32Value;
			case EConVarType_UInt32:	return value->m_u32Value;
			case EConVarType_Int64:		return value->m_i64Value;
			case EConVarType_UInt64:	return value->m_u64Value;
			case EConVarType_Float32:	return value->m_fl32Value;
			case EConVarType_Float64:	return value->m_fl64Value;
			default:					break;
		}
	}

	return *GetInvalidConVarData( TranslateConVarType<T>() )->DefaultValue();
}

template<typename T>
inline void CConVarRef<T>::Set( const T &value, CSplitScreenSlot slot )
{
	CVValue_t *cvvalue = m_ConVarData->Value( slot );

	if(cvvalue)
	{
		CVValue_t newval;
		TypeTraits()->Construct( &newval );
		TypeTraits()->Copy( &newval, value );

		SetOrQueueValueInternal( slot, &newval );

		TypeTraits()->Destruct( &newval );
	}
}

template<typename T>
inline void ConVarRefAbstract::SetAs( const T &value, CSplitScreenSlot slot )
{
	if(GetType() == TranslateConVarType<T>())
	{
		CConVarRef<T>( *this ).Set( value, slot );
	}
	else if(GetType() == EConVarType_String)
	{
		CBufferString buf;
		CVValue_t cvvalue( value );
		GetCvarTypeTraits( TranslateConVarType<T>() )->ValueToString( &cvvalue, buf );

		CConVarRef<CUtlString>( *this ).Set( buf.Get(), slot );
	}
	else if(IsValuePrimitive())
	{
		ConvertToPrimitiveFrom<T>( slot, value );
	}
}

template<> inline void ConVarRefAbstract::SetAs( const CUtlString &value, CSplitScreenSlot slot )
{
	SetString( value, slot );
}

template<typename T>
inline void ConVarRefAbstract::ConvertToPrimitiveFrom( CSplitScreenSlot slot, const T &value ) const
{
	if constexpr(CTypePOD<T>)
	{
		switch(GetType())
		{
			case EConVarType_Bool:		return CConVarRef<bool>( *this ).Set( value, slot );
			case EConVarType_Int16:		return CConVarRef<int16_t>( *this ).Set( value, slot );
			case EConVarType_UInt16:	return CConVarRef<uint16_t>( *this ).Set( value, slot );
			case EConVarType_Int32:		return CConVarRef<int32_t>( *this ).Set( value, slot );
			case EConVarType_UInt32:	return CConVarRef<uint32_t>( *this ).Set( value, slot );
			case EConVarType_Int64:		return CConVarRef<int64_t>( *this ).Set( value, slot );
			case EConVarType_UInt64:	return CConVarRef<uint64_t>( *this ).Set( value, slot );
			case EConVarType_Float32:	return CConVarRef<float>( *this ).Set( value, slot );
			case EConVarType_Float64:	return CConVarRef<double>( *this ).Set( value, slot );
			default:					break;
		}
	}
}

typedef void (*FnConVarRegisterCallback)(ConVarRefAbstract *ref);
typedef void (*FnConCommandRegisterCallback)(ConCommandRef *ref);

void ConVar_Register( uint64_t nCVarFlag = 0, FnConVarRegisterCallback cvar_reg_cb = nullptr, FnConCommandRegisterCallback cmd_reg_cb = nullptr );
void ConVar_Unregister( );

void ConVar_PrintDescription( const ConVarRefAbstract *ref );

#ifdef _MSC_VER
#pragma warning (disable : 4355 )
#endif

template< class T >
class CConCommandMemberAccessor : public ICommandCallback, public ICommandCompletionCallback, public ConCommand
{
	typedef ConCommand BaseClass;
	typedef void ( T::*FnMemberCommandCallback_t )( const CCommandContext &context, const CCommand &command );
	typedef int  ( T::*FnMemberCommandCompletionCallback_t )( const CCommand &command, CUtlVector< CUtlString > &completions );

public:
	CConCommandMemberAccessor( T* pOwner, const char *pName, FnMemberCommandCallback_t callback, const char *pHelpString,
		uint64_t flags = 0, FnMemberCommandCompletionCallback_t completionFunc = 0 ) :
		BaseClass( pName, this, pHelpString, flags, ( completionFunc != 0 ) ? this : NULL )
	{
		m_pOwner = pOwner;
		m_Func = callback;
		m_CompletionFunc = completionFunc;
	}

	~CConCommandMemberAccessor()
	{
		this->Destroy();
	}

	void SetOwner( T* pOwner )
	{
		m_pOwner = pOwner;
	}

	virtual void CommandCallback( const CCommandContext &context, const CCommand &command ) override
	{
		Assert( m_pOwner && m_Func );
		(m_pOwner->*m_Func)( context, command );
	}

	virtual int  CommandCompletionCallback( const CCommand &command, CUtlVector< CUtlString > &completions ) override
	{
		Assert( m_pOwner && m_CompletionFunc );
		return (m_pOwner->*m_CompletionFunc)( command, completions );
	}

private:
	T* m_pOwner;
	FnMemberCommandCallback_t m_Func;
	FnMemberCommandCompletionCallback_t m_CompletionFunc;
};

// AMNOTE: Shouldn't be used directly to create new concommands
class ConCommandRegList
{
public:
	friend void ConVar_Register( uint64_t nCVarFlag, FnConVarRegisterCallback cvar_reg_cb, FnConCommandRegisterCallback cmd_reg_cb );
	friend void ConVar_Unregister();
	friend void SetupConCommand( ConCommand *cmd, const ConCommandCreation_t &info );

	struct Entry_t
	{
		ConCommandCreation_t m_Info;
		ConCommandRef *m_Command = nullptr;
	};

private:
	static void RegisterConCommand( const Entry_t &cmd );
	static void RegisterAll();
	static void UnregisterAll();
	static void AddToList( const Entry_t &cmd );

public:
	uint32_t m_nSize;
	Entry_t m_Entries[100];
	ConCommandRegList *m_pPrev;

private:
	static bool s_bConCommandsRegistered;
	static ConCommandRegList *s_pRoot;
};

// AMNOTE: Shouldn't be used directly to create new convars
class ConVarRegList
{
public:
	friend void ConVar_Register( uint64_t nCVarFlag, FnConVarRegisterCallback cvar_reg_cb, FnConCommandRegisterCallback cmd_reg_cb );
	friend void ConVar_Unregister();
	friend void SetupConVar( ConVarRefAbstract *cvar, ConVarData **cvar_data, ConVarCreation_t &info );

	struct Entry_t
	{
		ConVarCreation_t m_Info;

		ConVarRefAbstract *m_pConVar = nullptr;
		ConVarData **m_pConVarData = nullptr;
	};

private:
	static void RegisterConVar( const Entry_t &cvar );
	static void RegisterAll();
	static void UnregisterAll();
	static void AddToList( const Entry_t &cvar );

public:
	uint32_t m_nSize;
	Entry_t m_Entries[100];
	ConVarRegList *m_pPrev;

private:
	static bool s_bConVarsRegistered;
	static ConVarRegList *s_pRoot;
};

#ifdef _MSC_VER
#pragma warning ( default : 4355 )
#endif

#endif