#ifndef IVSCRIPT_H
#define IVSCRIPT_H

#ifdef _WIN32
#pragma once
#endif

#include "public/appframework/iappsystem.h"
#include "public/tier0/utlvector.h"
#include "public/tier0/keyvalues.h"
#include "public/tier0/convar.h"

enum ScriptLanguage_t
{
	SL_NONE,
	SL_LUA,
	SL_NOP,

	SL_DEFAULT = SL_LUA
};

class IScriptVM;

class IScriptDebugger
{
public:
	virtual bool		StartDebugging( const char *pszIDEKey ) = 0;
	virtual void		StopDebugging() = 0;
	virtual bool		IsDebugging() = 0;
	virtual const char	*GetIDEKey() = 0;

	// Returns false when not debugging
	virtual bool		ConnectVM( IScriptVM *pVM ) = 0;
	virtual void		Update( IScriptVM *pVM ) = 0;
	virtual IScriptVM	*GetConnectedVM() = 0;
	virtual void		DisconnectVM( IScriptVM *pVM ) = 0;

	virtual void		HandleOutputMsg( IScriptVM *pVM, const char *pszMsg ) = 0;
	virtual void		HandleErrorMsg( IScriptVM *pVM, const char *pszMsg ) = 0;

	// A compiled or released script of pVM; the debugger lists the connected VM's scripts under "file:///" URLs
	// (pszChunkName is "@" and the script's file name)
	virtual void		OnScriptCompiled( IScriptVM *pVM, const char *pszScript, const char *pszChunkName, HSCRIPT hScript, void *pUnk ) = 0;
	virtual void		OnScriptReleased( IScriptVM *pVM, HSCRIPT hScript ) = 0;
	// A script error in the VM whose IScriptVM::GetInternalVM() is pInternalVM; pauses when it's the connected VM's and
	// the debugger asked to pause on uncaught exceptions
	virtual void		OnScriptError( void *pInternalVM, void *pUnk ) = 0;
	// Breaks in when the script with this file name (unqualified) runs, while debugging; null or empty clears it
	virtual void		SetBreakOnScript( const char *pszFileName ) = 0;
	// Pauses in the debugger, as the scripts' DebuggerBreak() does
	virtual void		Break() = 0;
};

class IScriptManager : public IAppSystem
{
public:
	virtual IScriptVM *CreateVM( ScriptLanguage_t language = SL_DEFAULT ) = 0;
	virtual void DestroyVM( IScriptVM * ) = 0;
	virtual IScriptDebugger *GetDebugger() = 0;
};

#include "public/entity2/variant.h"

#define INVALID_HSCRIPT ((HSCRIPT)-1)

typedef fieldtype_t ScriptDataType_t;
typedef CVariant ScriptVariant_t;

#define SCRIPT_VARIANT_NULL ScriptVariant_t()

template <typename T> struct ScriptDeducer { /*enum { FIELD_TYPE = FIELD_TYPEUNKNOWN };*/ };
#define DECLARE_DEDUCE_FIELDTYPE( fieldType, type ) template<> struct ScriptDeducer<type> { enum { FIELD_TYPE = fieldType }; };

DECLARE_DEDUCE_FIELDTYPE( FIELD_VOID, void );
DECLARE_DEDUCE_FIELDTYPE( FIELD_FLOAT32, float );
DECLARE_DEDUCE_FIELDTYPE( FIELD_CSTRING, const char * );
DECLARE_DEDUCE_FIELDTYPE( FIELD_CSTRING, char * );
DECLARE_DEDUCE_FIELDTYPE( FIELD_VECTOR, Vector );
DECLARE_DEDUCE_FIELDTYPE( FIELD_VECTOR, const Vector & );
DECLARE_DEDUCE_FIELDTYPE( FIELD_INT32, int32_t );
DECLARE_DEDUCE_FIELDTYPE( FIELD_BOOLEAN, bool );
DECLARE_DEDUCE_FIELDTYPE( FIELD_CHARACTER, char );
DECLARE_DEDUCE_FIELDTYPE( FIELD_HSCRIPT, HSCRIPT );
DECLARE_DEDUCE_FIELDTYPE( FIELD_VARIANT, ScriptVariant_t );

#define ScriptDeduceType( T ) ((ScriptDataType_t)ScriptDeducer<T>::FIELD_TYPE)

struct ScriptFuncDescriptor_t
{
	ScriptFuncDescriptor_t();

	const char *m_pszScriptName;
	const char *m_pszFunction;
	const char *m_pszDescription;
	ScriptDataType_t m_ReturnType;
	uint8_t m_iVariantCount;
	uint8_t m_iParamCount;
	ScriptDataType_t m_Parameters[12];

	// Any/all parameter names. Read as a buffer of null-termed strings.
	// If first is NULL / 0-len, no parameter names are present.
	const char *m_pszParameterNames;
};

// Prefix a script description with this in order to not show the function or class in help
#define SCRIPT_HIDE "@"

// Prefix a script description of a class to indicate it is a singleton and the single instance should be in the help
#define SCRIPT_SINGLETON "!"

// Prefix a script description with this to indicate it should be represented using an alternate name
#define SCRIPT_ALIAS( alias, description ) "#" alias ":" description

enum ScriptFuncBindingFlags_t
{
	SF_MEMBER_FUNC	= 0x01,
};

struct ScriptClassDesc_t;

typedef bool (*ScriptBindingFunc_t)( void *pFunction, void *pContext, ScriptVariant_t *pArguments, int nArguments, ScriptVariant_t *pReturn );

struct ScriptFunctionBinding_t
{
	ScriptFuncDescriptor_t	m_desc;
	ScriptClassDesc_t *		m_pClassDesc;
	ScriptBindingFunc_t		m_pfnBinding;
	void *					m_pFunction;
	ScriptFuncBindingFlags_t m_flags;
};

class IScriptInstanceHelper
{
public:
	virtual void *GetProxied( void *p );
	virtual bool ToString( void *p, char *pBuf, int bufSize );
	virtual void *BindOnRead( HSCRIPT hInstance, void *pOld, const char *pszId );
};

struct ScriptClassDesc_t
{
	ScriptClassDesc_t();

	const char *						m_pszScriptName;
	const char *						m_pszClassname;
	const char *						m_pszDescription;
	ScriptClassDesc_t *					m_pBaseDesc;
	CUtlVector<ScriptFunctionBinding_t> m_FunctionBindings;

	void *(*m_pfnConstruct)();
	void (*m_pfnDestruct)( void *);
	IScriptInstanceHelper *				pHelper; // optional helper
};

template <typename T>
class CScriptConstructor
{
public:
	static void *Construct()		{ return new T; }
	static void Destruct( void *p )	{ delete (T *)p; }
};

enum ScriptErrorLevel_t
{
	SCRIPT_LEVEL_WARNING	= 0,
	SCRIPT_LEVEL_ERROR,
};

typedef void ( *ScriptOutputFunc_t )( const char *pszText );
typedef bool ( *ScriptErrorFunc_t )( ScriptErrorLevel_t eLevel, const char *pszText );
typedef bool ( *ScriptKeyValuesFromTableFunc_t )( KeyValues *pKeyValues, const char *pszKey, const ScriptVariant_t &value, void *pContext );

#ifdef RegisterClass
#undef RegisterClass
#endif

enum ScriptStatus_t
{
	SCRIPT_ERROR = -1,
	SCRIPT_DONE,
	SCRIPT_RUNNING,
};

class IScriptVM
{
public:
	virtual ~IScriptVM();

	virtual bool Init() = 0;
	virtual void Shutdown() = 0;

	// AMNOTE: Makes ExecuteFunction fail with SCRIPT_ERROR from then on, Run is unaffected
	virtual void DisableExecuteFunction() = 0;

	virtual ScriptLanguage_t GetLanguage() = 0;
	virtual const char *GetLanguageName() = 0;
	
	virtual void *GetInternalVM() = 0;

	virtual void AddSearchPath( const char *pszSearchPath ) = 0;
	
	virtual void ClearTypeMap() = 0;
	
	virtual void EnableLocalDiskAccess() = 0;
	
	virtual bool ForwardConsoleCommand(const CCommandContext &, const CCommand &) = 0;

	//--------------------------------------------------------
 
 	virtual bool Frame( float simTime ) = 0;

	//--------------------------------------------------------
	// Simple script usage
	//--------------------------------------------------------
	virtual ScriptStatus_t Run( const char *pszScript, bool bWait = true ) = 0;
	ScriptStatus_t Run( const unsigned char *pszScript, bool bWait = true );

	//--------------------------------------------------------
	// Compilation
	//--------------------------------------------------------
 	virtual HSCRIPT CompileScript( const char *pszScript, const char *pszId = NULL ) = 0;
	HSCRIPT CompileScript( const unsigned char *pszScript, const char *pszId = NULL );
	virtual void ReleaseScript( HSCRIPT ) = 0;

	//--------------------------------------------------------
	// Execution of compiled
	//--------------------------------------------------------
	virtual ScriptStatus_t Run( HSCRIPT hScript, HSCRIPT hScope = NULL, bool bWait = true ) = 0;
	virtual ScriptStatus_t Run( HSCRIPT hScript, bool bWait ) = 0;

	// AMNOTE: Returns the script or function that Run or ExecuteFunction is currently running, NULL outside of them
	virtual HSCRIPT GetExecutingScript() = 0;

	//--------------------------------------------------------
	// Scope
	//--------------------------------------------------------
	virtual HSCRIPT CreateScope( const char *pszScope, HSCRIPT hParent = NULL ) = 0;
	virtual HSCRIPT ReferenceScope( HSCRIPT hScript ) = 0;
	virtual void ReleaseScope( HSCRIPT hScript ) = 0;

	//--------------------------------------------------------
	// Script functions
	//--------------------------------------------------------
	virtual HSCRIPT LookupFunction( const char *pszFunction, HSCRIPT hScope = NULL, bool raw = false ) = 0;
	virtual void ReleaseFunction( HSCRIPT hScript ) = 0;

	//--------------------------------------------------------
	// Script functions (raw, use Call())
	//--------------------------------------------------------
	virtual ScriptStatus_t ExecuteFunction( HSCRIPT hFunction, ScriptVariant_t *pArgs, int nArgs, ScriptVariant_t *pReturn, HSCRIPT hScope, bool bWait ) = 0;

	//--------------------------------------------------------
	// External functions
	//--------------------------------------------------------
	virtual void RegisterFunction( ScriptFunctionBinding_t *pScriptFunction ) = 0;

	//--------------------------------------------------------
	// External classes
	//--------------------------------------------------------
	virtual bool RegisterScriptClass( ScriptClassDesc_t *pClassDesc ) = 0;

	//--------------------------------------------------------
	// External instances. Note class will be auto-registered.
	//--------------------------------------------------------

	virtual HSCRIPT RegisterInstance( ScriptClassDesc_t *pDesc, void *pInstance ) = 0;
	virtual void SetInstanceUniqeId( HSCRIPT hInstance, const char *pszId ) = 0;
	template <typename T> HSCRIPT RegisterInstance( T *pInstance )																	{ return RegisterInstance( GetScriptDesc( pInstance ), pInstance );	}
	template <typename T> HSCRIPT RegisterInstance( T *pInstance, const char *pszInstance, HSCRIPT hScope = NULL)					{ HSCRIPT hInstance = RegisterInstance( GetScriptDesc( pInstance ), pInstance ); SetValue( hScope, pszInstance, hInstance ); return hInstance; }
	virtual void RemoveInstance( HSCRIPT ) = 0;
	void RemoveInstance( HSCRIPT hInstance, const char *pszInstance, HSCRIPT hScope = NULL );
	void RemoveInstance( const char *pszInstance, HSCRIPT hScope = NULL );

	virtual void *GetInstanceValue( HSCRIPT hInstance, ScriptClassDesc_t *pExpectedType = NULL ) = 0;

	//----------------------------------------------------------------------------

	virtual bool GenerateUniqueKey( const char *pszRoot, char *pBuf, int nBufSize ) = 0;

	//----------------------------------------------------------------------------

	virtual bool ValueExists( HSCRIPT hScope, const char *pszKey ) = 0;
	bool ValueExists( const char *pszKey );

	virtual bool SetValue( HSCRIPT hScope, const char *pszKey, const char *pszValue ) = 0;
	virtual bool SetValue( HSCRIPT hScope, const char *pszKey, const ScriptVariant_t &value ) = 0;
	virtual bool SetValue( HSCRIPT hScope, int nIndex, const ScriptVariant_t &value ) = 0;

	bool SetValue( const char *pszKey, const ScriptVariant_t &value );

	virtual bool SetEnumValue(HSCRIPT hScope, const char *pszEnumName, const char *pszValueName, int value, const char *pszDescription) = 0;

	virtual void CreateTable( ScriptVariant_t &Table ) = 0;
	virtual bool IsTable( HSCRIPT hScope ) = 0;
	virtual int	GetNumTableEntries( HSCRIPT hScope ) = 0;
	virtual int GetNumElements( HSCRIPT hScope ) = 0;
	virtual int GetKeyValue( HSCRIPT hScope, int nIterator, ScriptVariant_t *pKey, ScriptVariant_t *pValue ) = 0;
	// The caller owns the returned KeyValues. pfnCallback may be NULL, keys it returns true for are not added
	virtual KeyValues *CreateKeyValuesFromTable(HSCRIPT hScope, const char* pszName, ScriptKeyValuesFromTableFunc_t pfnCallback, void* pContext) = 0;

	virtual bool GetValue( HSCRIPT hScope, const char *pszKey, ScriptVariant_t *pValue ) = 0;
	virtual bool GetValue( HSCRIPT hScope, int nIndex, ScriptVariant_t *pValue ) = 0;
	bool GetValue( const char *pszKey, ScriptVariant_t *pValue );
	virtual bool GetScalarValue( HSCRIPT hScope, ScriptVariant_t *pValue ) = 0;
	virtual void CopyValue( const ScriptVariant_t &src, ScriptVariant_t *pDest ) = 0;
	virtual void ReleaseValue( ScriptVariant_t &value ) = 0;

	virtual bool ClearValue( HSCRIPT hScope, const char *pszKey ) = 0;
	bool ClearValue( const char *pszKey);
	
	virtual void CreateArray( ScriptVariant_t &Array ) = 0;
	virtual bool IsArray( HSCRIPT hScope ) = 0;
	virtual int GetArrayCount( HSCRIPT hScope ) = 0;
	virtual int ArrayAddToTail( HSCRIPT hScope, const ScriptVariant_t &pValue ) = 0;
	
	//----------------------------------------------------------------------------

	virtual void WriteState( CUtlBuffer *pBuffer ) = 0;
	virtual void ReadState( CUtlBuffer *pBuffer ) = 0;
	
	virtual void CollectGarbage( const char *, bool ) = 0;

	virtual void DumpState() = 0;

	virtual void SetOutputCallback( ScriptOutputFunc_t pFunc ) = 0;
	virtual void SetErrorCallback( ScriptErrorFunc_t pFunc ) = 0;

	//----------------------------------------------------------------------------

	virtual bool RaiseException( const char *pszExceptionText ) = 0;

	virtual HSCRIPT GetRootTable() = 0;

	virtual HSCRIPT CopyHandle( HSCRIPT hScope ) = 0;

	// Returns 0 on success
	virtual int LoadAndCompileScriptFile( const char *pszFile, const char *pszPathId, HSCRIPT *pScript ) = 0;

	virtual void GetSourceId( HSCRIPT hScope, char *pBuf, int nBufSize ) = 0;

	virtual bool AreHandlesEqual( HSCRIPT hScript1, HSCRIPT hScript2 ) = 0;
};

#endif